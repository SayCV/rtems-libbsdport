#define DEVICET_EXTERN_INLINE

#include "devicet.h"
#include <rtems/rtems_bsdnet.h>
#include <sys/malloc.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <rtems/pci.h>
#include <rtems/error.h>
#include <sys/bus.h>
#include "libbsdport_api.h"

#define  DEBUG 0
int libbsdportAttachVerbose = DEBUG;

extern void real_libc_free(void*);

static STAILQ_HEAD(devq_t, device) devq = STAILQ_HEAD_INITIALIZER(devq);

static device_t
devalloc(driver_t *dr)
{
void     *m;
device_t rval;
int      l = sizeof(*rval) + dr->softc_size + DEVICE_SOFTC_ALIGNMENT -1;

	if ( !(m = malloc( l, M_DEVBUF, M_NOWAIT )) )
		return 0;

	memset(m, 0, l);

	rval = (device_t)(((uintptr_t)m + (DEVICE_SOFTC_ALIGNMENT-1)) & ~(DEVICE_SOFTC_ALIGNMENT-1));
	rval->rawmem = m;
	rval->type   = dr->type;
	rval->name   = dr->name;
	rval->drv    = dr; 

	return rval;
}

static void
devclean(device_t dev)
{
	assert( !dev->attached );
	memset(device_get_softc(dev), 0, dev->drv->softc_size);
	real_libc_free(dev->desc);
	dev->desc = 0;
	dev->unit = 0;
	dev->nameunit[0]=0;
	memset( &dev->bushdr, 0, sizeof(dev->bushdr));
}

static void
devfree(device_t dev)
{
	/* paranoia */
	devclean(dev);
	dev->drv = 0;
	free(dev->rawmem, M_DEVBUF);
}

static int
devattach(device_t dev, int unit, struct rtems_bsdnet_ifconfig *cfg)
{
int error;

	if ( libbsdportAttachVerbose ) {
		printf("Now attaching %s%d: (0x%x:%x.%x)\n",
			dev->name, unit, 
			dev->bushdr.pci.bus, dev->bushdr.pci.dev, dev->bushdr.pci.fun);
	}

	dev->unit     = unit;
	dev->ifconfig = cfg;
	sprintf(dev->nameunit,"%s%d",dev->drv->name,unit);

	/* Try to attach */
	if ( (error = dev->drv->methods->attach(dev)) ) {
		fprintf(stderr,"Attaching '%s%d' failed: %s", dev->drv->name, unit, strerror(error));
		return error;
	}
	/* Successfully attached new device */
	dev->attached = 1;
	cfg->name     = (char*)device_get_nameunit(dev);
	STAILQ_INSERT_TAIL(&devq, dev, list);
	return 0;
}

static int
devequal(device_t a, device_t b)
{
	if ( a->type != b->type )
		return 0;
	switch ( a->type ) {
		case DEV_TYPE_PCI:
			return  a->bushdr.pci.bus == b->bushdr.pci.bus
			     && a->bushdr.pci.dev == b->bushdr.pci.dev
			     && a->bushdr.pci.fun == b->bushdr.pci.fun;

		default:
			rtems_panic("devequal: Unsupported device type %i\n", a->type);
	}
	return 0;
}

/* Check if a particular device is already listed */
static device_t
devattached(device_t dev)
{
struct device *ldev;
	STAILQ_FOREACH(ldev, &devq, list) {
		if ( devequal(ldev, dev) )
			return ldev;
	}
	return 0;
}


int
device_printf(device_t dev, const char *fmt, ...)
{
int rval;
va_list ap;
	rval  = fprintf(stdout,"%s:",device_get_nameunit(dev));
	va_start(ap, fmt);
	rval += vfprintf(stdout,fmt,ap);
	va_end(ap);
	return rval;
}

static uint32_t
get_pci_triple(const char *drvnam)
{
unsigned b,d,f;
	if ( drvnam && 3 == sscanf(drvnam,"%i:%i.%i",&b,&d,&f) )
		return (b<<8) | PCI_DEVFN(d,f);
	return -1;
}

static void
get_name_unit(const char *drvnam, char *nm, int *punit)
{
int l = strlen(drvnam);
int i;
	if ( l > 0 ) {
		for ( i=l-1; i>=0 && isdigit(((unsigned char)drvnam[i])); i-- )
			/* nothing else to do */;
		if ( 1 != sscanf(drvnam+i,"%d",punit) )
			*punit = 0; /* wildcard */
		strncpy(nm, drvnam, i+1);
		nm[i+1]=0;
	} else {
		/* wildcards */
		*nm    = 0;
		*punit = 0;
	}
}

static int
matches(driver_t *dr, const char *pat)
{
	if ( 0 == *pat || '*' == *pat )
		return 1;
	return !strcmp(pat, dr->name);
}

static int
pci_slot_empty(int b, int d, int f)
{
uint16_t id;
	pci_read_config_word(b,d,f,PCI_VENDOR_ID,&id);
	return ( 0xffff == id );
}

static int
pci_is_ether(int b, int d, int f)
{
uint16_t dclass;
	if ( pci_slot_empty(b,d,f) )
		return 0;
	pci_read_config_word(b,d,f,PCI_CLASS_DEVICE, &dclass);
	return PCI_CLASS_NETWORK_ETHERNET == dclass;
}

/* this catches the case of an unpopulated slot (returning 0) */
static int
pci_num_functions(int b, int d)
{
uint8_t h;
	if ( pci_slot_empty(b,d,0) )
		return 0;
	pci_read_config_byte(b,d,0,PCI_HEADER_TYPE,&h);
	return (h & 0x80) ? PCI_MAX_FUNCTIONS : 1; /* multifunction device ? */
}

int
libbsdport_netdriver_dump(FILE *f)
{
struct device *ldev;
int           ndevs;
unsigned      w;

	if ( !f )
		f = stdout;

	ndevs = 0;
	fprintf(f, "PCI Network device information:\n");
	rtems_bsdnet_semaphore_obtain();
	STAILQ_FOREACH(ldev, &devq, list) {
		/* ASSUME LIST ELEMENTS DO NOT DISAPPEAR
		 * so we can release the lock while printing...
		 */
		rtems_bsdnet_semaphore_release();
		w=fprintf(f,"%-6s -- (0x%x:%x.%x)",
				device_get_nameunit(ldev),
				ldev->bushdr.pci.bus,
				ldev->bushdr.pci.dev,
				ldev->bushdr.pci.fun);
		for ( ; w < 24 ; w++)
			fputc(' ',f);
		if ( ldev->desc )
			fprintf(f," %s",ldev->desc);
		fputc('\n',f);


		ndevs++;
		rtems_bsdnet_semaphore_obtain();
	}
	rtems_bsdnet_semaphore_release();
	return ndevs;
}

#define UNITMATCH(wanted, unit, bdfunit) \
	((wanted) < 0 ? ((wanted) & 0xffff) == (bdfunit) : (wanted) == (unit))

int
libbsdport_netdriver_attach(struct rtems_bsdnet_ifconfig *cfg, int attaching)
{
char     nm[20]; /* copy of the name */
int      unit,thisunit,wantedunit;
int      i,b,d,f;
int      prob;
driver_t *dr;
device_t dev   = 0;
device_t tmpdev;
int      error = 0;
int      bdfunit;

int      n_bus;

		if ( !attaching )
			return ENOTSUP;

		if ( (wantedunit = get_pci_triple(cfg->name)) < 0 ) {
			get_name_unit(cfg->name, nm, &wantedunit);
		} else {
			wantedunit |= 1<<31;
			nm[0]=0;
		}
		if ( libbsdportAttachVerbose )
			printf("Wanted unit is 0x%x, pattern '%s'\n", wantedunit, nm);

		n_bus = pci_bus_count();
#ifdef __i386__
		/* ugliest of all hacks -- RTEMS routine is currently (4.9)
         * still broken; it reports the (0-based) highest bus number
         * instead of the count.
         */
		n_bus++;
#endif

		unit = 0;
		for ( i=0; (dr=libbsdport_netdriver_table[i]); i++ ) {

			/* unused slot ? */
			if ( 0 == dr->name && 0 == dr->methods )
				continue;

			/* Find matching driver */
			if ( libbsdportAttachVerbose )
				printf("Trying driver '%s' ...", dr->name);

			if ( matches(dr, nm) ) {

				if ( libbsdportAttachVerbose )
					printf("MATCH\n");

				assert( dr->methods );

				thisunit = 0;

				if ( DEV_TYPE_PCI != dr->type ) {
					fprintf(stderr,"Non-PCI driver '%s' not supported; skipping\n", dr->name);
					continue;
				}

				dev = devalloc(dr);
				for ( b=0; b<n_bus; b++ )
					for ( d=0; d<PCI_MAX_DEVICES; d++ ) {
						for ( f=0; f<pci_num_functions(b,d); f++ ) {
							if ( ! pci_is_ether(b,d,f) )
								continue;

							dev->bushdr.pci.bus = b;
							dev->bushdr.pci.dev = d;
							dev->bushdr.pci.fun = f;

							bdfunit = (b<<8) | PCI_DEVFN(d,f);

							if ( libbsdportAttachVerbose ) {
								printf("Probing PCI 0x%x:%x.%x\n",
										bdfunit>>8, PCI_SLOT(bdfunit), PCI_FUNC(bdfunit));
							}

							/* has this device been attached already ? */
							if ( (tmpdev = devattached(dev)) ) {
								if ( dev->drv == tmpdev->drv )
									thisunit++;
								unit++;
								if ( UNITMATCH(wantedunit, unit, bdfunit) ) {
									fprintf(stderr,"Device '%s' has already been attached\n", device_get_nameunit(dev));
									error = EBUSY;
									goto bail;
								}
							} else {
								switch ( ( prob = dr->methods->probe(dev) ) ) {
										/* LOW_PRIORITY currently unsupported; list preferred drivers first */
									case BUS_PROBE_LOW_PRIORITY:
									case BUS_PROBE_DEFAULT:
										/* accepted */
										thisunit++;
										unit++;
										/* wanted unit == 0 means next avail.
										 * unit is acceptable.
										 */
										if ( libbsdportAttachVerbose )
											printf("->SUCCESS\n");

										if ( 0 == wantedunit || UNITMATCH(wantedunit, unit, bdfunit) ) {
												error = devattach(dev, thisunit, cfg);
												if ( !error )
													dev = 0; /* is now on list */
												goto bail;
										}
										break;

									default:
										if ( libbsdportAttachVerbose )
											printf("->FAILED\n");
										/* probe failed */
										break;
								}
							}
							devclean(dev);
						} /* for all functions */
					} /* for all busses + slots */
				devfree(dev); dev = 0;
			} /* matching driver */
			else
			{
				if ( libbsdportAttachVerbose )
					printf("NO MATCH\n");
			}
		} /* for all drivers */

		/* Nothing found */
		error = ENODEV;
bail:
	if (dev)
		devfree(dev);
	return error;
}

device_t
libbsdport_netdriver_get_dev(const char *name)
{
struct device *ldev;

	if ( !name )
		return 0;

	rtems_bsdnet_semaphore_obtain();
		STAILQ_FOREACH(ldev, &devq, list) {
			if ( !strcmp(name, device_get_nameunit(ldev)) )
				break;
		}
	rtems_bsdnet_semaphore_release();
	return ldev;
}

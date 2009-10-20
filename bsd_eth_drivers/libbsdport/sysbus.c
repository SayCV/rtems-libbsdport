#include <rtems.h>
#include <rtems/pci.h>
#include <rtems/error.h>
#include <sys/errno.h>
#include <bsp.h>
#include <devicet.h>
#include <bsp/irq.h>
#include <rtems/irq.h>

#include <sys/taskqueue.h>

#include <sys/bus.h>
#include <sys/malloc.h>

#include <bsp/rtems_verscheck.h>

#if !RTEMS_REV_AT_LEAST(4,6,99) || !defined(BSP_SHARED_HANDLER_SUPPORT)

#include <bsp/bspExt.h>

#else

static void noop(const rtems_irq_connect_data *unused) {};
static int  noop1(const rtems_irq_connect_data *unused) { return 0;};

/* Finally have an ISR arg but the API still sucks.. */
static int
bspExtInstallSharedISR(int irqLine, void (*isr)(void *), void * uarg, int flags)
{
rtems_irq_connect_data suck = {0};
	suck.name   = irqLine;
	suck.hdl    = isr;
	suck.handle = uarg;
	suck.on     = noop;
	suck.off    = noop;
	suck.isOn   = noop1;
	return ! BSP_install_rtems_shared_irq_handler(&suck);
}

static int
bspExtRemoveSharedISR(int irqLine, void (*isr)(void *), void *uarg)
{
rtems_irq_connect_data suck = {0};
	suck.name   = irqLine;
	suck.hdl    = isr;
	suck.handle = uarg;
	suck.on     = noop;
	suck.off    = noop;
	suck.isOn   = noop1;
	return ! BSP_remove_rtems_irq_handler(&suck);
}
#endif


struct resource *
bus_alloc_resource_any(device_t dev, int type, int *prid, unsigned flags)
{
bus_addr_t ba;
int        isio;
	switch ( type ) {
		default:
			break;
		case SYS_RES_IOPORT:
		case SYS_RES_MEMORY:
			{
			libbsdport_u32_t d;
			pci_read_config_dword(
				dev->bushdr.pci.bus,
				dev->bushdr.pci.dev,
				dev->bushdr.pci.fun,
				*prid,
				&d);
			ba = d;
			isio = (ba & PCI_BASE_ADDRESS_SPACE_IO) ? 1 : 0;
			if ( (type == SYS_RES_IOPORT) != (isio != 0) )
				return 0;	/* wrong type */

			return (struct resource *) ba;
			}
		case SYS_RES_IRQ:
			{
			uint8_t line;
			pci_read_config_byte(
				dev->bushdr.pci.bus,
				dev->bushdr.pci.dev,
				dev->bushdr.pci.fun,
				PCI_INTERRUPT_LINE,
				&line);
			ba = line;
			/* MSI not implemented */
			return (struct resource*) ba;
			}
	}
	rtems_panic("bus_alloc_resource_any: unknown/unimplemented resource type %i\n", type);
	/* never get here */
	return (struct resource*)0;
}

int
bus_alloc_resources(device_t dev, struct resource_spec *rs,
    struct resource **res)
{
	int i;

	for (i = 0; rs[i].type != -1; i++)
		res[i] = NULL;
	for (i = 0; rs[i].type != -1; i++) {
		res[i] = bus_alloc_resource_any(dev,
		    rs[i].type, &rs[i].rid, rs[i].flags);
		if (res[i] == NULL && !(rs[i].flags & RF_OPTIONAL)) {
			bus_release_resources(dev, rs, res);
			return (ENXIO);
		}
	}
	return (0);
}

void
bus_release_resources(device_t dev, const struct resource_spec *rs,
    struct resource **res)
{
	int i;

	for (i = 0; rs[i].type != -1; i++)
		if (res[i] != NULL) {
			bus_release_resource(
			    dev, rs[i].type, rs[i].rid, res[i]);
			res[i] = NULL;
		}
}



struct irq_cookie {
	device_t        dev;
	driver_filter_t handler;
	void			(*work)(void*);
	void            *arg;
	/* cache methods */
	int				(*irq_check_dis)(device_t d);
	void			(*irq_en)       (device_t d);
	struct task     task;
};

static int
sysbus_isr(void *arg)
{
struct irq_cookie *info = arg;
int rval;
#ifdef DEBUG
	printk("Sysbus IRQ\n");
#endif
	/* Check if we have an IRQ pending and disable further interrupts */
	rval = info->irq_check_dis(info->dev);
	if ( FILTER_HANDLED == rval ) {
		/* enqueue work */
		taskqueue_enqueue(taskqueue_fast, &info->task);
	}
	return rval;
}

static void
sysbus_taskfn(void *arg, int pending)
{
struct irq_cookie *info = arg;
	
	/* do work */
	info->work(info->arg);
	
	/* reenable interrupts */
	if ( info->irq_en )
		info->irq_en(info->dev);
}

int
bus_setup_intr(device_t dev, struct resource *r, int flags, driver_filter_t filter, driver_intr_t handler, void *arg, void **cookiep)
{
int                rval;
struct irq_cookie *info = 0;

	if ( filter && handler ) {
		rtems_panic("bus_setup_intr for both: filter & handler not implemented\n");
	}

	if ( handler ) {
		if ( !dev->drv ) {
			device_printf(dev, "bus_setup_intr: device has no driver attached\n");
			return EINVAL;
		} else if ( !dev->drv->methods->irq_check_dis ) {
			device_printf(dev, "bus_setup_intr: driver has no 'irq_check_dis' method\n");
			return EINVAL;
		}
	}

	if ( ! (info = malloc(sizeof(*info), M_DEVBUF, M_NOWAIT)) )
		return ENOMEM;

	info->dev     = dev;
	info->handler = filter;
	info->work    = handler;
	info->arg     = arg;

	if ( handler ) {
		TASK_INIT(&info->task, 0, sysbus_taskfn, info);
		/* make sure taskqueue facility is initialized */
		rtems_taskqueue_initialize();
		/* install our own filter */
		filter = sysbus_isr;
		arg    = info;
		info->irq_check_dis = dev->drv->methods->irq_check_dis;
		info->irq_en        = dev->drv->methods->irq_en;
	} else {
		TASK_INIT(&info->task, 0, 0, 0);
	}

	rval = bspExtInstallSharedISR((int)r, (void (*)(void*))filter, arg, 0);

	if ( rval ) {
		free(info, M_DEVBUF);
		return rval;
	}

	*cookiep = info;
	return rval;
}

int
bus_teardown_intr(device_t dev, struct resource *r, void *cookiep)
{
int rval;
struct irq_cookie *info = cookiep;
	rval = bspExtRemoveSharedISR((int)r, (void (*)(void*))info->handler, info->arg);
	if ( 0 == rval ) {
		if ( info->task.ta_fn ) {
			taskqueue_drain(taskqueue_fast, &info->task);
		}
		free(info, M_DEVBUF);
	}
	return rval;
}

bus_space_handle_t
rman_get_bushandle(struct resource *r)
{
bus_space_handle_t h = (bus_space_handle_t)r;
bus_space_handle_t msk = (PCI_BASE_ADDRESS_SPACE_IO & h) ? PCI_BASE_ADDRESS_IO_MASK : PCI_BASE_ADDRESS_MEM_MASK;
	return h & msk;
}

bus_space_tag_t
rman_get_bustag(struct resource *r)
{
bus_space_handle_t h = (bus_space_handle_t)r;
	return (PCI_BASE_ADDRESS_SPACE_IO & h) ? bus_space_io : bus_space_mem;
}

int
bus_dma_tag_create(void *parent, unsigned alignment, unsigned bounds, uint32_t lowadd, uint32_t hiaddr, void (*filter)(void*), void *filterarg, unsigned maxsize, int nsegs, unsigned maxsegsize, unsigned flags, void (*lockfunc)(void*), void *lockarg, bus_dma_tag_t *ptag)
{
bus_dma_tag_t tag;
	if ( filter || lockfunc )
		return ENOTSUP;
	if ( ! (tag = malloc(sizeof(*tag), M_DEVBUF, M_NOWAIT)) )
		return ENOMEM;
	/* save some information */
	tag->alignment = alignment;
	tag->maxsize   = maxsize;
	tag->maxsegs   = nsegs;
	*ptag          = tag;
	return 0;
}

void
bus_dma_tag_destroy(bus_dma_tag_t tag)
{
	free(tag, M_DEVBUF);
}

int
bus_dmamem_alloc(bus_dma_tag_t tag, void **p_vaddr, unsigned flags, bus_dmamap_t *p_map)
{
uintptr_t a;
unsigned  sz = tag->maxsize + tag->alignment;
	if ( ! (*p_map = malloc(sz, M_DEVBUF, M_NOWAIT)) )
		return ENOMEM;
	a = ((uintptr_t)*p_map + tag->alignment - 1 ) & ~(tag->alignment - 1);
	*p_vaddr = (void*)a;
	if ( (BUS_DMA_ZERO & flags) )
		memset(*p_map, 0, sz);
	return 0;
}

void
bus_dmamem_free(bus_dma_tag_t tag, void *vaddr, bus_dmamap_t map)
{
	free(map, M_DEVBUF);
}

/* Dummy handle for Giant mutex */
uint32_t __busdma_dummy_Giant = 0;

int
resource_int_value(const char *name, int unit, const char *resname, int *result)
{
	/* not implemented */
	return ENOENT;
}
int
resource_long_value(const char *name, int unit, const char *resname, long *result)
{
	/* not implemented */
	return ENOENT;
}

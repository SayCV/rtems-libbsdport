#ifndef RTEMS_NETDEV_T_DECL_H
#define RTEMS_NETDEV_T_DECL_H

#include <rtems.h>
#include <rtems/bspIo.h>
#include <stdarg.h>
#include <stdio.h>

#include <bsp/rtems_verscheck.h>

#if RTEMS_REV_AT_LEAST(4,8,99)
#include <rtems/bsd/sys/queue.h>
#else
#include <sys/queue.h>
#endif

#include <libbsdport_api.h>

/* se we can generate a non-inlined version somewhere */
#ifndef DEVICET_EXTERN_INLINE
#define DEVICET_EXTERN_INLINE extern inline
#endif

/* unused for now: */
typedef int            devclass_t;

typedef struct device *device_t;

typedef struct _pcidev_t {
	unsigned short bus;
	unsigned char  dev;
	unsigned char  fun;
} pcidev_t;

#define DEV_TYPE_PCI	1

typedef int device_probe_t   (device_t);
typedef int device_attach_t  (device_t);
typedef int device_detach_t  (device_t);
typedef int device_resume_t  (device_t);
typedef int device_suspend_t (device_t);

typedef struct device_methods {
	int	 (*probe )         (device_t);	
	int	 (*attach)         (device_t);	
	void (*shutdown)       (device_t);	
	int	 (*detach)         (device_t);	
	int  (*irq_check_dis)  (device_t);
	void (*irq_en)         (device_t);
} device_method_t;

struct driver {
	const char      *name;
	device_method_t *methods;
	int             type;
	int             softc_size;
};

#define DEVICE_SOFTC_ALIGNMENT 16

struct device {
	union {
		pcidev_t	pci;
	}	     bushdr;
	int      type;
	STAILQ_ENTRY(device) list;
	const char     *name;
	char     nameunit[16];	/* NEVER use knowledge about the size of this -- we may change it */
	int      unit;
	char     *desc;
	driver_t *drv;
	int      attached;
	void     *rawmem;       /* back pointer */
	struct rtems_bsdnet_ifconfig *ifconfig;
	char softc[] __attribute__ ((aligned(DEVICE_SOFTC_ALIGNMENT), may_alias));
	/* a pointer to back to the device is installed past the 'softc' */
};

static inline device_t
rtems_softc2dev(void *softc)
{
uintptr_t diff = (uintptr_t)&((device_t)(0))->softc - (uintptr_t)(device_t)(0);
	return (device_t)((uintptr_t)softc - diff);
}

#define device_set_desc_copy(dev, nm)	\
	do { real_libc_free((dev)->desc); (dev)->desc = strdup((nm)); } while (0)

#define device_set_desc(dev, nm) device_set_desc_copy(dev, nm)

static inline const char *
device_get_nameunit(device_t dev)
{
	return dev->nameunit;
}

static inline const char *
device_get_name(device_t dev)
{
	return dev->name;
}

static inline int
device_get_unit(device_t dev)
{
	return dev->unit;
}

DEVICET_EXTERN_INLINE void *
device_get_softc(device_t dev)
{
	return dev->softc;
}

#define device_delete_child(dev,bus) do {} while (0)

static inline int
device_is_attached(device_t dev)
{
	return dev->attached;
}

int device_printf(device_t dev, const char *fmt, ...);

#endif

#include "devicet.h"
#include <rtems/rtems_bsdnet.h>

struct {
	struct device dev;
	struct {
		char space[4096];
	} softc;
} thele = {
	{
	bushdr: {
/* mvme5500 		{ x, x, x } */
/* qemu     */ { 0, 3, 0 }
	},
	type: DEV_TYPE_PCI,
	name: "le",
	nameunit: { 'l', 'e', '1', 0},
	unit: 1,
	},
	{
	{ 0, }
	}
};

void *thelesoftc = &thele.softc;


struct {
	struct device dev;
	struct {
		char space[4096];
	} softc;
} theem = {
	{
	bushdr: {
/* mvme5500 		{ 2, 0xa, 0 } */
/* cpci     */ { 7, 0, 0 }
	},
	type: DEV_TYPE_PCI,
	name: "em",
	nameunit: { 'e', 'm', '1', 0},
	unit: 1,
	},
	{
	{ 0, }
	}
};

void *theemsoftc = &theem.softc;

struct {
	struct device dev;
	struct {
		char space[4096];
	} softc;
} thepcn = {
	{
	bushdr: {
/* mvme5500 		{ x, 0xx, x } */
/* cpci     */ { 4, 6, 0 }
	},
	type: DEV_TYPE_PCI,
	name: "pcn",
	nameunit: { 'p', 'c', 'n', '1', 0},
	unit: 1,
	},
	{
	{ 0, }
	}
};

void *thepcnsoftc = &thepcn.softc;

extern driver_t rtems_em_driver;
extern driver_t rtems_le_pci_driver;
extern driver_t rtems_pcn_driver;

driver_t *rtems_netdriver_table[] = {
	&rtems_em_driver,
	&rtems_le_pci_driver,
	&rtems_pcn_driver,
	0
};

struct rtems_bsdnet_ifconfig pcncfg = {
	name: "pcn",
	rbuf_count:20,
	xbuf_count:3,
};

#ifdef DEBUG_MODULAR
void
_cexpModuleInitialize(void *unused)
{
extern void * rtems_callout_initialize();
extern void * rtems_taskqueue_initialize();
	rtems_callout_initialize();
	rtems_taskqueue_initialize();
}
#endif

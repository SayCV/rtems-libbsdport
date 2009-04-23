#include <stdio.h>
#include "libbsdport_api.h"
#include "devicet.h"

driver_t *libbsdport_netdriver_table_all[] = {
	&libbsdport_em_driver,
	&libbsdport_pcn_driver,
	&libbsdport_le_pci_driver,
	&libbsdport_fxp_driver,
	&libbsdport_bge_driver,
	&libbsdport_re_driver,
	0
};

driver_t libbsdport_null_driver = {0};

extern driver_t libbsdport_em_driver
	__attribute__((weak,alias("libbsdport_null_driver")));
extern driver_t libbsdport_pcn_driver
	__attribute__((weak,alias("libbsdport_null_driver")));
extern driver_t libbsdport_le_pci_driver
	__attribute__((weak,alias("libbsdport_null_driver")));
extern driver_t libbsdport_fxp_driver
	__attribute__((weak,alias("libbsdport_null_driver")));
extern driver_t libbsdport_bge_driver
	__attribute__((weak,alias("libbsdport_null_driver")));
extern driver_t libbsdport_re_driver
	__attribute__((weak,alias("libbsdport_null_driver")));


/* weak alias defaults to a table that includes
 * all currently supported drivers.
 *
 * However, the individual entires are weak aliases
 * themselves so that you don't have to link all drivers...
 */
extern driver_t *libbsdport_netdriver_table
	[
	sizeof(libbsdport_netdriver_table_all)/sizeof(libbsdport_netdriver_table_all[0])
	] __attribute__((weak,alias("libbsdport_netdriver_table_all")));

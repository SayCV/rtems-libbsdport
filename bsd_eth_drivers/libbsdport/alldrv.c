#include <stdio.h>
#include "libbsdport_api.h"

driver_t *libbsdport_netdriver_table_all[] = {
	&libbsdport_em_driver,
	&libbsdport_pcn_driver,
	&libbsdport_le_pci_driver,
	0
};

/* weak alias defaults to a table that includes all currently supported drivers */
extern driver_t *libbsdport_netdriver_table
	[
	sizeof(libbsdport_netdriver_table_all)/sizeof(libbsdport_netdriver_table_all[0])
	] __attribute__((weak,alias("libbsdport_netdriver_table_all")));

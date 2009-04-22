#ifndef LIBBSDPORT_API_H
#define LIBBSDPORT_API_H

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>

/* $Id$ */

/* User API to libbsdport driver attach function, driver table etc. */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct driver driver_t;

/* NULL terminated list of all drivers configured into the system.
 * To be defined by the application.
 */
extern driver_t *libbsdport_netdriver_table[];

/* Drivers ported so far: */
/* Intel E1000 chips */
extern driver_t libbsdport_em_driver;       
/* AMD 79C971..976 pcnet PCI */
extern driver_t libbsdport_pcn_driver;
/* AMD/Lance older (and later) chips; this driver also supports what 'pcn'
 * does but might not be as efficient.
 * NOTE: The 'le_pci' driver works with the pcnet32 (79C970A) emulation
 *       of qemu.
 */
extern driver_t libbsdport_le_pci_driver;


/* Generic driver attach function (can be used in rtems_bsdnet_ifconfig).
 * This routine selects a driver/device combination based on
 *  - drivers available / listed in libbsdport_netdriver_table[];
 *  - devices detected in PCI config space compatible with a listed
 *    driver.
 *  - name and unit specified in the rtems_bsdnet_ifconfig.name field
 *    (empty name: "" is a wildcard).
 *
 * E.g. assume that
 *  1) the 'em' and 'pcn' drivers are listed in the table.
 *  2) a AMD Am79C973 chip is somewhere on the PCI bus
 *  3) ifconfig name is ""
 *  -> the 'pcn' driver is selected and the only AMD chip present
 *     is used as 'pcn1'.
 *  -> If the name was 'em' or 'pcn2' no device would be found
 *     (no em device found; no 2nd pcn device found).
 *
 * Now assume that you have a 82544 and two AMD 79C973 devices:
 *
 *  name: ""     picks the first of the three chips found in PCI space
 *  name: "pcn"  picks the first AMD chip found
 *  name: "em"   picks the first (and only) 82544
 *  name: "pcn2" picks the second AMD chip
 *
 * Also, it is possible to specify a PCI-triple: <busno>:<slotno>.<fnno>
 * i.e., name: "2:3.0" tries to find a driver that supports the device
 * at bus #2, slot #2.
 *
 * NOTE: detaching a driver is not supported (since rtems bsdnet cannot detach
 * an interface).
 */
int
libbsdport_netdriver_attach(struct rtems_bsdnet_ifconfig *cfg, int attaching);

/* Print information about all attached drivers to FILE (stdout if NULL)
 *
 * RETURNS: number of devices attached so far.
 *
 * BUGS:    more info should be printed.
 */
int
libbsdport_netdriver_dump(FILE *f);

#ifdef __cplusplus
}
#endif

#endif

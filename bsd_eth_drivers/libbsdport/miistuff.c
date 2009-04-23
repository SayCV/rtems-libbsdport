#include <rtems.h>
#define _KERNEL
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include <bsp/rtems_verscheck.h>

#include <sys/malloc.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <net/if_media.h>
#include <rtems/rtems_mii_ioctl.h>

#define PHY_MAX 32

#undef  DEBUG

/* A helper to find the active PHY. We really should port
 * the entire BSD miibus/phy support but that's a bigger
 * project...
 */
int
rtems_mii_phy_probe(struct rtems_mdio_info *mdio, void *softc)
{
int      phy;
uint32_t bmsr, bmcr;
	for ( phy = 0; phy<PHY_MAX; phy++ ) {
		if ( mdio->mdio_r(phy, softc, MII_BMSR, &bmsr) )
			continue;

		bmsr &= 0xffff;

		if ( 0 == bmsr || 0xffff == bmsr )	
			continue; /* nothing here */

		/* no media supported ? */
		if ( 0 == ((BMSR_EXTSTAT | 0xfe00) & bmsr ) )
			continue; /* probably nothing there */

		if ( mdio->mdio_r(phy, softc, MII_BMCR, &bmcr) )
			continue;

		/* skip isolated or powered-down phys */
		if ( (BMCR_PDOWN | BMCR_ISO) & bmcr )
			continue;

#ifdef DEBUG
		printk("PHY #%u seems active; link status is %s\n", phy, BMSR_LINK & bmsr ? "UP" : "DOWN");
#endif

		/* seems we found one */
		return phy;
	}

	return -1;
}

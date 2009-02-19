
#include "libbsdport.h"

#define _KERNEL
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_types.h>
#include <net/ethernet.h>

static struct ifnet *rtems_bsdnet_if_freelist = 0;

struct ifnet *
if_alloc(int type)
{
struct ifnet *rval = 0;
	if ( IFT_ETHER == type ) {
		/* Hack with freelist allows for debugging drivers as modules */
		if ( (rval = rtems_bsdnet_if_freelist) ) {
			/* use softc pointer to link free list */
			rtems_bsdnet_if_freelist = rtems_bsdnet_if_freelist->if_softc;
		}
		if ( (rval = malloc(sizeof(struct arpcom), M_DEVBUF, M_WAIT)) )
			memset(rval, 0, sizeof(struct arpcom));
	}
	return rval;
}

void
if_free(struct ifnet *ifp)
{
	/* save on free-list so subsequent alloc gets old
	 * interface back (which is still on the bsdnet stack's list
	 * of known interfaces. The old rtems stack doesn't provide 
	 * means to remove an interface once it has been attached.
	 * This hack allows for detaching a *driver* and reattaching
	 * it to the same interface later (good for development/debugging).
	 */
	ifp->if_softc = rtems_bsdnet_if_freelist;
	rtems_bsdnet_if_freelist = ifp;
}

/* Ugly hack to allow unloading/reloading the driver core.
 * Needed because rtems' bsdnet release doesn't implement
 * if_detach(). Therefore, we bring the interface down but
 * keep the device record alive...
 */
void
ether_ifdetach(struct ifnet *ifp)
{
	(ifp)->if_flags = 0;
	(ifp)->if_ioctl = 0; 
	(ifp)->if_start = 0;
	(ifp)->if_watchdog = 0;
	(ifp)->if_init  = 0;
}


/* copy ethernet addr into arpcom if nothing is set yet */
void
ether_setaddr(struct ifnet *ifp, u_int8_t *eaddr)
{
int i;
device_t dev = rtems_softc2dev(ifp->if_softc);
	/* If LLADDR has already been set, then use it */
	for ( i=0; i< ETHER_ADDR_LEN; i++ ) {
		if ( IF_LLADDR(ifp)[i] )
			break;
	}
	if ( i >= ETHER_ADDR_LEN ) {
		/* not set; see if the ifconfig struct provides one */
		if ( dev->ifconfig && dev->ifconfig->hardware_address )
			memcpy(IF_LLADDR(ifp), dev->ifconfig->hardware_address, ETHER_ADDR_LEN);
		else
			memcpy(IF_LLADDR(ifp), eaddr, ETHER_ADDR_LEN);
	}
}

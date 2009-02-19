#include <rtems.h>
#define _KERNEL
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include <bsp/rtems_verscheck.h>

#if RTEMS_REV_AT_LEAST(4,8,99)
#include <rtems/bsd/sys/queue.h>
#else
#include <sys/queue.h>
#endif

#include <sys/malloc.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <net/if_media.h>
#include <rtems/rtems_mii_ioctl.h>

void
ifmedia_init(struct ifmedia *ifm, int dontcare_mask,
             ifm_change_cb_t change_callback, ifm_stat_cb_t status_callback)
{
	ifm->ifm_mask  = dontcare_mask;
	ifm->ifm_media = 0;
	ifm->ifm_cur   = 0;
	ifm->ifm_list.lh_first = NULL;
	ifm->ifm_change = change_callback;
	ifm->ifm_status = status_callback;
}

void
ifmedia_add(struct ifmedia *ifm, int mword, int data, void *aux)
{
struct ifmedia_entry *ifmen, *ifmep, *ifme;
	if ( ( ifme = malloc(sizeof(*ifme), M_DEVBUF, M_NOWAIT) ) ) {
		ifme->ifm_media = mword;
		ifme->ifm_data  = data;
		ifme->ifm_aux   = aux;
		for ( ifmep = LIST_FIRST(&ifm->ifm_list); ifmep; ifmep = ifmen ) {
			if ( !(ifmen = LIST_NEXT(ifmep, ifm_list)) )
				break;
		}
		if ( ifmep )
			LIST_INSERT_AFTER(ifmep, ifme, ifm_list);
		else
			LIST_INSERT_HEAD( &ifm->ifm_list, ifme, ifm_list);
	}
}

int
ifmedia_ioctl(struct ifnet *ifp, struct ifreq *ifr, struct ifmedia *ifm, u_long cmd)
{
int               rval = 0;
struct ifmediareq ifmr;
	if ( SIOCGIFMEDIA == cmd ) {
		if ( !ifm->ifm_status )
			return ENOTSUP;
		ifm->ifm_status(ifp, &ifmr);
		if ( ! (IFM_AVALID & ifmr.ifm_status) )
			return EINVAL;
		/* translate */
		ifr->ifr_media = ifmr.ifm_active;
		if ( IFM_ACTIVE & ifmr.ifm_status )
			ifr->ifr_media |= IFM_LINK_OK;
		/* no way to determine if autoneg is forcefully disabled
		 * from ifmr :-(
		 * Look at current ifm_media for now.
		 */
		if ( IFM_SUBTYPE(ifm->ifm_media) != IFM_AUTO )
			ifr->ifr_media |= IFM_ANEG_DIS;
	} else {
		if ( !ifm->ifm_change )
			return ENOTSUP;
		ifm->ifm_media = ifr->ifr_media;
		rval = ifm->ifm_change(ifp);
	}
	return rval;
}

void
ifmedia_set(struct ifmedia *ifm, int mword)
{
	ifm->ifm_media = mword;
	/* cannot invoke the ifm_change callback because we have
	 * no ifp here.
	 */
}


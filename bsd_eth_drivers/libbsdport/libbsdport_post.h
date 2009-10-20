/* This file is to be included from 'if_xxx.c' AFTER all other
 * includes so we can override some rtems-bsdnet things...
 */
#if 1
/* These are defined in sys/sysctl.h and we
 * could one day add support ...
 */
#undef  SYSCTL_ADD_PROC
#define SYSCTL_ADD_PROC(unused...) do { } while (0)

#undef  SYSCTL_ADD_INT
#define SYSCTL_ADD_INT(unused...)  do { } while (0)

#define RTEMS_SYSCTL_NOTYETSUP
#endif

#include <netinet/in.h>
#include <netinet/if_ether.h>

/* include after <net/if.h> & friends */
#include <rtems/rtems_mii_ioctl.h>

#define if_drv_flags if_flags
#define IFF_DRV_RUNNING	IFF_RUNNING
#define IFF_DRV_OACTIVE IFF_OACTIVE

static inline struct mbuf *
m_getcl(int how, int type, unsigned flags)
{
struct mbuf *mp = 0;
	if ( ! (flags & M_PKTHDR) ) {
		printk("m_getcl: DUNNO WHAT TO DO HERE\n");
		return 0;
	}
	MGETHDR(mp, M_DONTWAIT, MT_DATA);
	if ( mp ) {
		MCLGET( mp, M_DONTWAIT );
		if ( !(mp->m_flags & M_EXT) ) {
			m_freem(mp);
			mp = 0;
		}
	}
	return mp;	
}

static inline void
ether_input_skipping(struct ifnet *ifp, struct mbuf *m)
{	struct ether_header *eh;
	eh				  = mtod(m, struct ether_header*);
#if 1
	m_adj(m, sizeof(struct ether_header));
#else
	/* faster hack */
   	m->m_data        += sizeof(struct ether_header);
   	m->m_len         -= sizeof(struct ether_header);
   	m->m_pkthdr.len  -= sizeof(struct ether_header);
#endif
	/* some drivers don't set this */
	m->m_pkthdr.rcvif = ifp;
   	ether_input(ifp, eh, m);
}

void
ether_setaddr(struct ifnet *ifp, u_int8_t *eaddr);

#define ether_ifattach(ifp, eaddr) \
	do { \
		(ifp)->if_output = ether_output; \
		if ( !(ifp)->if_addrlist ) {	\
			/* reattach hack; do this only the first time -- detaching is not implemented, however!! */ \
			ether_setaddr(ifp, eaddr); \
			if_attach(ifp); \
			ether_ifattach(ifp); \
		} \
	} while (0)


/* Not 100% sure this is correct */
#define M_MOVE_PKTHDR(to, from) \
	do { \
		(to)->m_flags = ((from)->m_flags & M_COPYFLAGS) | ((to)->m_flags & M_EXT); \
		if (((to)->m_flags & M_EXT) == 0) \
			(to)->m_data = (to)->m_pktdat; \
		(to)->m_pkthdr   = (from)->m_pkthdr; \
		(from)->m_flags &= ~M_PKTHDR; \
	} while (0)

#define ETHER_SIOCMULTIFRAG(e, c, ifr, ifp)                    \
	( ((e) = (SIOCADDMULTI == (c) ?                            \
			ether_addmulti((ifr), (struct arpcom*)(ifp)) :     \
			ether_delmulti((ifr), (struct arpcom*)(ifp)) )),   \
	  ((e) = ENETRESET == (e) ? 0 : (e)) )

#define arp_ifinit(ifp, ifa) arp_ifinit((struct arpcom *)ifp, ifa)


#define mii_mediachg(mii) do {} while (0)

/* find the currently active PHY; assume bootloader or firmware
 * has set it up already.
 *
 * RETURNS: phy idx 0..31 or -1 on error
 */
int
rtems_mii_phy_probe(struct rtems_mdio_info *mdio, void *if_softc);

/*
 * Defragment a mbuf chain, returning the shortest possible
 * chain of mbufs and clusters.  If allocation fails and
 * this cannot be completed, NULL will be returned, but
 * the passed in chain will be unchanged.  Upon success,
 * the original chain will be freed, and the new chain
 * will be returned.
 *
 * If a non-packet header is passed in, the original
 * mbuf (chain?) will be returned unharmed.
 */
struct mbuf *
m_defrag(struct mbuf *m0, int how);


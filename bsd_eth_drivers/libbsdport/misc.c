
#include <libbsdport.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/taskqueue.h>
#include <sys/mbuf.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>

#include <libbsdport_post.h>

int libbsdport_bootverbose = 0;

#ifdef WITNESS
#define MBUF_CHECKSLEEP(how) do {\
  if (how == M_WAITOK)\
  WITNESS_WARN(WARN_GIANTOK | WARN_SLEEPOK, NULL,\
               "Sleeping in \"%s\"", __func__);\
  } while (0)
#else
#define MBUF_CHECKSLEEP(how)
#endif

#define MBTOM(how)(how)

u_int
m_length(struct mbuf *m0, struct mbuf **last)
{
  struct mbuf *m;
  u_int len;

  len = 0;
  for (m = m0; m != NULL; m = m->m_next) {
    len += m->m_len;
    if (m->m_next == NULL)
      break;
  }
  if (last != NULL)
    *last = m;
  return (len);
}

/*
 * Duplicate "from"'s mbuf pkthdr in "to".
 * "from" must have M_PKTHDR set, and "to" must be empty.
 * In particular, this does a deep copy of the packet tags.
 */
int
m_dup_pkthdr(struct mbuf *to, struct mbuf *from, int how)
{

  #if 0
  /*
   * The mbuf allocator only initializes the pkthdr
   * when the mbuf is allocated with MGETHDR. Many users
   * (e.g. m_copy*, m_prepend) use MGET and then
   * smash the pkthdr as needed causing these
   * assertions to trip.  For now just disable them.
   */
  M_ASSERTPKTHDR(to);
  /* Note: with MAC, this may not be a good assertion. */
  KASSERT(SLIST_EMPTY(&to->m_pkthdr.tags), ("m_dup_pkthdr: to has tags"));
  #endif
  MBUF_CHECKSLEEP(how);
  #ifdef MAC
  if (to->m_flags & M_PKTHDR)
    m_tag_delete_chain(to, NULL);
  #endif
  to->m_flags = (from->m_flags & M_COPYFLAGS) | (to->m_flags & M_EXT);
  if ((to->m_flags & M_EXT) == 0)
    to->m_data = to->m_pktdat;
  to->m_pkthdr = from->m_pkthdr;
  return 1;
}

u_int
m_fixhdr(struct mbuf *m0)
{
  u_int len;

  len = m_length(m0, NULL);
  m0->m_pkthdr.len = len;
  return (len);
}

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
m_defrag(struct mbuf *m0, int how)
{
  struct mbuf *m_new = NULL, *m_final = NULL;
  int progress = 0, length;

  MBUF_CHECKSLEEP(how);
  if (!(m0->m_flags & M_PKTHDR))
    return (m0);

  m_fixhdr(m0); /* Needed sanity check */

  #ifdef MBUF_STRESS_TEST
  if (m_defragrandomfailures) {
    int temp = arc4random() & 0xff;
    if (temp == 0xba)
      goto nospace;
  }
  #endif

  if (m0->m_pkthdr.len > MHLEN)
    m_final = m_getcl(how, MT_DATA, M_PKTHDR);
  else
    m_final = m_gethdr(how, MT_DATA);

  if (m_final == NULL)
    goto nospace;

  if (m_dup_pkthdr(m_final, m0, how) == 0)
    goto nospace;

  m_new = m_final;

  while (progress < m0->m_pkthdr.len) {
    length = m0->m_pkthdr.len - progress;
    if (length > MCLBYTES)
      length = MCLBYTES;

    if (m_new == NULL) {
      if (length > MLEN)
        m_new = m_getcl(how, MT_DATA, 0);
      else
        m_new = m_get(how, MT_DATA);
      if (m_new == NULL)
        goto nospace;
    }

    m_copydata(m0, progress, length, mtod(m_new, caddr_t));
    progress += length;
    m_new->m_len = length;
    if (m_new != m_final)
      m_cat(m_final, m_new);
    m_new = NULL;
  }
  #ifdef MBUF_STRESS_TEST
  if (m0->m_next == NULL)
    m_defraguseless++;
  #endif
  m_freem(m0);
  m0 = m_final;
  #ifdef MBUF_STRESS_TEST
  m_defragpackets++;
  m_defragbytes += m0->m_pkthdr.len;
  #endif
  return (m0);
  nospace:
  #ifdef MBUF_STRESS_TEST
  m_defragfailure++;
  #endif
  if (m_final)
    m_freem(m_final);
  return (NULL);
}


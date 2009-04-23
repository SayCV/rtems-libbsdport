#ifndef _RTEMS_BSDNET_MUTEX_H
#define _RTEMS_BSDNET_MUTEX_H

/* NOTE: mutexes should never be necessary since
 * the RTEMS BSD code protects everything with a
 * big fat lock
 */

struct mtx {
	rtems_id mtx_id;
};

#define MTX_DEF	    0 /* default sleeping lock */
#define MTX_RECURSE	4 /* nesting */

#define MTX_NETWORK_LOCK	"xxx"

static inline void
mtx_init(struct mtx *m, const char *name, const char *type, int opts)
{
	/* Set ID to zero in case they want to submit this mutex
	 * to callout_init_mtx()
	 */
	m->mtx_id = 0;
}

static inline int
mtx_initialized(struct mtx *m)
{
	return m->mtx_id == 0;	
}

static inline void
mtx_lock(struct mtx *m)
{
}

static inline void
mtx_unlock(struct mtx *m)
{
}

static inline void
mtx_destroy(struct mtx *m)
{
}

/* what ? */
#define MA_OWNED    1
#define MA_NOTOWNED 0
static inline void
mtx_assert(struct mtx *m, int what)
{
}

#endif

#ifndef _SYS_CALLOUT_H
#define _SYS_CALLOUT_H /* include this to override rtems stack's */

/* RTEMS systm.h still declares old timout stuff which is not
 *       fully compatible with more recent 'callout' functionality.
 *
 *       Also: our struct callout it incompatible with the one
 *             declared in rtems' sys/callout.h.
 *             Make sure to include the proper header (first).
 */

typedef unsigned callout_time_t;

struct callout {
	struct callout  *c_next;
	struct callout **c_pprev;
	void           (*c_func)(void*);
	void            *c_arg;
	struct mtx      *c_mtx;
	callout_time_t   c_time; 
	unsigned         c_flags;
};

#define CALLOUT_PENDING (1<<0)
#define CALLOUT_ACTIVE  (1<<1)

/*
 * Strictly, we don't need any protection
 * because the global network semaphore
 * takes care; however we want to 
 */
static inline int
callout_active(struct callout *p_c)
{
int rval;
rtems_interrupt_level l;
	rtems_interrupt_disable(l);
		rval = p_c->c_flags & CALLOUT_ACTIVE;
	rtems_interrupt_enable(l);
	return rval;	
}

static inline int
callout_pending(struct callout *p_c)
{
int rval;
rtems_interrupt_level l;
	rtems_interrupt_disable(l);
		rval = p_c->c_flags & CALLOUT_PENDING;
	rtems_interrupt_enable(l);
	return rval;	
}

static inline void
callout_decativate(struct callout *p_c)
{
rtems_interrupt_level l;
	rtems_interrupt_disable(l);
		p_c->c_flags &= ~CALLOUT_ACTIVE;
	rtems_interrupt_enable(l);
}

/* We cannot stop a callout that's in progress */

int
callout_stop(struct callout *c);

#define callout_drain callout_stop

int
callout_reset(struct callout *c, int ticks, void (*fn)(void*), void *arg);

void
callout_init(struct callout *c, int mpsafe);

void
callout_init_mtx(struct callout *c, struct mtx *m, unsigned flags);

/* Initialize callout facility [networking must have been initialized already] */
rtems_id
rtems_callout_initialize();

#endif

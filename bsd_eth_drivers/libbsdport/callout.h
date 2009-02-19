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
};

/* We cannot stop a callout that's in progress */

void
callout_stop(struct callout *c);

#define callout_drain callout_stop

void
callout_reset(struct callout *c, int ticks, void (*fn)(void*), void *arg);

void
callout_init(struct callout *c, int mpsafe);

void
callout_init_mtx(struct callout *c, struct mtx *m, unsigned flags);

/* Initialize callout facility [networking must have been initialized already] */
rtems_id
rtems_callout_initialize();

#endif

#include <rtems.h>
#include <rtems/error.h>
#include <string.h>

#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include "mutex.h"
#include "callout.h"

#define STATIC static

/* Implementation modelled after
 *
 * "Redesign the BSD Callout and Timer Facilities"
 * Adam M. Costello, George Varghese
 * Dpt. of Computer Science, Washington University, 1995.
 */

#include <assert.h>

/* rely on networking semaphore for now */
#define LIST_KEY_DECL(k) 
#define LIST_LOCK(k)   do {} while (0)
#define LIST_UNLOCK(k) do {} while (0)

#define WHEELBITS 5

#define WHEELMASK ((1<<(WHEELBITS))-1)

#define CALLOUT_EVENT	RTEMS_EVENT_1
#define KILL_EVENT		RTEMS_EVENT_2


typedef void (*timeout_t)(void*);

STATIC volatile callout_time_t hard_ticks = 0;
STATIC volatile callout_time_t soft_ticks = 0;

STATIC struct callout *c_wheel[1<<WHEELBITS] = {0};

static inline void
c_enq(struct callout **where, struct callout *c)
{
	assert( c->c_pprev == 0 && c->c_next == 0 );
	if ( (c->c_next = *where) )
		(*where)->c_pprev = &c->c_next;
	c->c_pprev = where;
	*where     = c;
}

static inline void
c_deq(struct callout *c)
{
struct callout *n;
	assert( c->c_pprev );
	if ( (n = *c->c_pprev = c->c_next) )
		n->c_pprev = c->c_pprev;
	c->c_next = 0;
	c->c_pprev = 0;
}

static inline void
softclock()
{
struct callout        *c, *n;
rtems_interrupt_level  k1;
callout_time_t         st,ht;
LIST_KEY_DECL(k);

	/* I believe this is free of a race condition (softclock
	 * and hardclock both update volatile 'soft_ticks' variable):
	 *  a) 'hardclock' runs at IRQ level and is atomic
	 *  b) inside while loop 'soft_ticks' is != 'hard_ticks'
	 *  c) hardclock only modifies soft_ticks if 'soft_ticks'=='hard_ticks'
	 *     hence this could only happen just after the update of 'soft_ticks'
	 *     at the end of the while loop completes.
	 */

	while ( 1 ) {
	/* Must atomically read 'soft_ticks' and 'hard_ticks' -- otherwise,
	 * hardclock might update both but we get one old and one new value
	 */
	rtems_interrupt_disable(k1);
		st = soft_ticks;
		ht = hard_ticks;
	rtems_interrupt_enable(k1);
		if ( st == ht )
			break; /* caught up */

		/* at this point, we know that st != ht and therefore,
		 * hardclock will only increment hard_ticks but leave
		 * soft_ticks alone.
		 */

		st++;

		LIST_LOCK(k);
		for ( c = c_wheel[ st & WHEELMASK ]; c; c=n ) {
			n = c->c_next;
			if ( c->c_time <= 0 ) {
				/* this one expired */
				rtems_interrupt_disable(k1);
					c->c_flags &= ~ CALLOUT_PENDING;
				rtems_interrupt_enable(k1);
				c_deq(c);
				if ( c->c_func )
					c->c_func(c->c_arg);
			} else {
				c->c_time--;
			}
		}
		LIST_UNLOCK(k);
		soft_ticks = st;
		/* here, soft_ticks could have caught up and
		 * a hardclock occurring here could also
		 * update soft_ticks.
		 */
	}
}

static inline void
hardclock(rtems_id tid)
{
	if ( hard_ticks++ == soft_ticks && !c_wheel[hard_ticks & WHEELMASK] ) {
		/* nothing to do */
		soft_ticks++;
	} else {
		rtems_event_send(tid, CALLOUT_EVENT);
	}
}

static void
calloutTick(rtems_id myself, void *arg)
{
rtems_id tid = (rtems_id)arg;

	hardclock(tid);

	rtems_timer_fire_after(myself, 1, calloutTick, arg);
}

static void
calloutTask(void *arg)
{
rtems_event_set   ev;
rtems_status_code sc;
rtems_id          ticker = 0;
rtems_id          me;

	sc = rtems_timer_create(rtems_build_name('b','s','d','c'), &ticker);
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_error(sc, "Creation of timer failed\n");
		goto bail;
	}
	rtems_task_ident(RTEMS_SELF, RTEMS_LOCAL, &me);

	rtems_timer_fire_after(ticker, 1, calloutTick, (void*)me);

	while ( 1 ) {
		sc = rtems_bsdnet_event_receive (CALLOUT_EVENT | KILL_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &ev);
		if ( RTEMS_SUCCESSFUL != sc ) {
			rtems_error(sc, "calloutTask: unable to receive event; terminating\n");
			break;
		}
		if ( ev & KILL_EVENT ) {
			break;
		}
		softclock();
	}
bail:
	rtems_timer_delete(ticker);
	rtems_task_delete(RTEMS_SELF);
}


/* We cannot stop a callout that's in progress */

int
callout_stop(struct callout *c)
{
rtems_interrupt_level l;
LIST_KEY_DECL(k);

	if ( !c->c_pprev )
		return 0;	/* not currently on a list */

	LIST_LOCK(k);
		/* have to check again */
		if ( ! c->c_pprev ) {
			LIST_UNLOCK(k);
			return 0;
		}
		/* remove from list */
		c_deq(c);
		rtems_interrupt_disable(l);
		c->c_flags &= ~(CALLOUT_ACTIVE | CALLOUT_PENDING);
		rtems_interrupt_enable(l);
	LIST_UNLOCK(k);

	return 1;
}


int
callout_reset(struct callout *c, int ticks, timeout_t fn, void *arg)
{
rtems_interrupt_level l;
LIST_KEY_DECL(k);
int                 i, rval;

	if ( ticks <= 0 )
		ticks = 1;

	rval = callout_stop(c);

	c->c_func = fn;
	c->c_arg  = arg;

	LIST_LOCK(k);
	i         = (hard_ticks + ticks) & WHEELMASK;
	c->c_time = ticks >> WHEELBITS;

	/* enqueue */
	c_enq(&c_wheel[i], c);

	rtems_interrupt_disable(l);
		c->c_flags |= (CALLOUT_ACTIVE | CALLOUT_PENDING);
	rtems_interrupt_enable(l);

	LIST_UNLOCK(k);

	return rval;
}

static rtems_id callout_tid = 0;

void
callout_init(struct callout *c, int mpsafe)
{
	/* non thread-safe lazy init in case nobody cared to do it ... */
	if ( !callout_tid )
		rtems_callout_initialize();
	memset(c,0,sizeof(*c));	
}

void
callout_init_mtx(struct callout *c, struct mtx *m, unsigned flags)
{
	if ( m->mtx_id )
		rtems_panic("callout_init_mtx: using mutex not supported\n");
	callout_init(c,0);
	c->c_mtx = m;
}

rtems_id
rtems_callout_initialize()
{
	if ( !callout_tid )
		callout_tid=rtems_bsdnet_newproc ("cout", 4096, calloutTask, NULL);
	return callout_tid;
}

#ifdef DEBUG
void
_cexpModuleInitialize(void*u)
{
	rtems_bsdnet_initialize_network();
	rtems_callout_initialize();
}
#endif

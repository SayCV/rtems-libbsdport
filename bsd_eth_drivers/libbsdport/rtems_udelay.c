#include <rtems.h>
#include <rtems/error.h>
#include <bsp.h>

#include "rtems_udelay.h"

#if defined(__PPC__)

#include <rtems/powerpc/registers.h>

/* Ouch. stupid bookE doesn't implement mftb so we must
 * use mfspr (which wouldn't work on classic ppc if we
 * were in user mode but luckily we're not.
 */
static inline uint64_t __read_hires_clicks()
{
uint32_t tbl, tbu1, tbu2;
	asm volatile(
		"	mfspr %0, %4	\n"
		"	mfspr %1, %3	\n"
		"	mfspr %2, %4	\n"
		:"=r"(tbu1),"=r"(tbl),"=r"(tbu2)
		:"i"(TBRL), "i"(TBRU)
		);
	if ( tbu1 != tbu2 )
		asm volatile("mfspr %0, %1":"=r"(tbl):"i"(TBRL));
	return ((uint64_t)tbu2<<32) | tbl;
}

#define __rtems_hires_kHz (BSP_bus_frequency/BSP_time_base_divisor)

#elif defined(__i386__)

static inline uint64_t __read_hires_clicks()
{
uint32_t lo,hi;
	asm volatile("rdtsc":"=a"(lo),"=d"(hi));
	return ( (uint64_t)hi << 32 ) | lo;
}

#else
#error "rtems_udelay.c not ported to this CPU yet"
#endif

#ifndef __rtems_hires_kHz
/* Clock frequency of high-resolution timer */
uint32_t __rtems_hires_kHz = 0;
uint32_t rtems_udelay_calibrate();
#endif


void rtems_usec_delay(uint32_t usecs)
{
uint64_t clicks = __read_hires_clicks();
int      ticks;
	if (usecs > 10) {
		if ( _ISR_Is_in_progress() ) {
			rtems_panic("rtems_usec_delay for more than 10us in ISR!!");
		}
		if ( _ISR_Get_level() > 0 ) {
			rtems_panic("rtems_usec_delay for more than 10us with IRQs disabled!!");
		}
	}
	ticks   = usecs/rtems_configuration_get_microseconds_per_tick();
#ifndef __rtems_hires_kHz
	/* If it's not a macro; do lazy init */
	if ( 0 == __rtems_hires_kHz ) {
		uint64_t clicks = rtems_udelay_calibrate();
		__rtems_hires_kHz = (clicks * 1000) / rtems_configuration_get_microseconds_per_tick();
		ticks--;
	}
#endif
	clicks += (usecs * __rtems_hires_kHz)/1000;
	if ( ticks > 0 )
		rtems_task_wake_after(ticks);

	while ( clicks > __read_hires_clicks() )
		/* busy wait */;
}

/* This doesn't belong here; also, the RTEMS timeout() implementation is buggy:
 * if a timeout is added when the networking task is asleep then I believe 'timeout()'
 * is unable to schedule a wakeup. Therefore, I implemented the 'callout' facility.
 */
#ifdef UNTESTED
/* Must be executed with the network semaphore held */
void
rtems_bsdnet_untimeout(timeout_func_t fn, void *arg)
{
register struct callout *l, *p;

	for ( l = &calltodo; (p=l->c_next); l=p ) {
		if ( p->c_func == fn && p->c_arg == arg ) {
			register struct callout *n
			/* found it */
			if ( (n = p->c_next) && p->c_time > 0 ) {
				/* adjust time of following entry */
				n->c_time += p->c_time;
			}
			/* extract */
			l->c_next = n;
			/* return to extract first occurrence; continue
			 * to extract all
			 */
#if 0
			return;
#else
			p = l;
#endif
		}
	}
}
#endif

struct caldat {
	uint64_t t0,t1,t2,t3;
	rtems_id thetid;
};

#ifndef __rtems_hires_kHz
static void
tickmeas1(rtems_id myself, void *arg)
{
struct caldat *p = arg;
	p->t2 = __read_hires_clicks();
	rtems_event_send(p->thetid, RTEMS_EVENT_0);
}

static void
tickmeas0(rtems_id myself, void *arg)
{
struct caldat *p = arg;
	p->t1 = __read_hires_clicks();
	rtems_timer_fire_after( myself, 1, tickmeas1, arg );
}

/* Calibrate high-resolution timer */
uint32_t
rtems_udelay_calibrate()
{
rtems_id          timer;
rtems_status_code sc;
rtems_event_set   ev;
struct caldat     d;

	/* measure a clock tick with the hires timer;
	 * note that we can't just sleep for 1 tick because
	 * that results in sleeping for an unknown fraction
	 * of a tick...
	 */
	sc = rtems_timer_create( rtems_build_name('h','r','e','s'), &timer );
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_panic("Unable to create timer:%i\n", sc);
	}
	sc = rtems_task_ident(RTEMS_SELF, RTEMS_LOCAL, &d.thetid);
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_panic("Unable to read my own TID:%i\n", sc);
	}
#ifdef DEBUG
	d.t0 = __read_hires_clicks();
#endif
	sc = rtems_timer_fire_after(timer, 1, tickmeas0, &d);
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_panic("Unable to fire timer:%i\n", sc);
	}
	sc = rtems_event_receive( RTEMS_EVENT_0, RTEMS_EVENT_ANY | RTEMS_WAIT , RTEMS_NO_TIMEOUT, &ev); 
#ifdef DEBUG
	d.t3 = __read_hires_clicks();
#endif
	if ( RTEMS_SUCCESSFUL != sc ) {
		rtems_panic("Unable to synchronize with timer:%i\n", sc);
	}
	rtems_timer_delete(timer);
#ifdef DEBUG
	printf("Diffs: %llu %llu %llu\n",
	d.t3-d.t2, d.t2-d.t1, d.t1-d.t0);
#endif
	return d.t2-d.t1;
}
#endif

#ifdef DEBUG
unsigned
hdiff(unsigned s)
{
uint64_t now = __read_hires_clicks();
	rtems_task_wake_after(s);
	now = __read_hires_clicks() - now;
	printf("Diff was %llu clicks\n",now);
	return (unsigned)now;
}
#endif

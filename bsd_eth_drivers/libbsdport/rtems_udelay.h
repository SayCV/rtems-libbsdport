#ifndef RTEMS_UDELAY_Y
#define RTEMS_UDELAY_Y

#ifdef __cplusplus
extern "C" {
#endif

/* Delay execution for n microseconds. The current task
 * is suspended for multiples of OS 'ticks' and busy-waits
 * for fractions thereof.
 * The routine panics if requested to delay for more than
 * 10us in an ISR or IRQ-disabled section of code.
 */
void rtems_usec_delay(uint32_t usecs);

#define DELAY(usecs) rtems_usec_delay(usecs)

#ifdef __cplusplus
}
#endif

#endif

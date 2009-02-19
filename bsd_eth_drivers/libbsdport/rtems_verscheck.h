#ifndef RTEMS_VERSION_CHECKER_H
#define RTEMS_VERSION_CHECKER_H
/* $Id$ Macros to check rtems version dependent API features :-( */

#include <rtems.h>

#define RTEMS_REV_LATER_THAN(ma,mi,re) \
	(    __RTEMS_MAJOR__  > (ma)	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__  > (mi))	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__ == (mi) && __RTEMS_REVISION__ > (re)) \
    )

#define RTEMS_REV_AT_LEAST(ma,mi,re) \
	(    __RTEMS_MAJOR__  > (ma)	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__  > (mi))	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__ == (mi) && __RTEMS_REVISION__ >= (re)) \
    )

/*
 * unfortunately, (powerpc) libcpu/io.h didn't follow the change from 
 * unsigned -> uin32_t :--(
 */
#if RTEMS_REV_AT_LEAST(4,8,0)
typedef uint32_t libbsdport_u32_t;
#else
typedef unsigned libbsdport_u32_t;
#endif

#endif

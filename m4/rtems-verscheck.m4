# Emit two macros
#
#   'RTEMS_VERSION_LATER_THAN(major,minor,revision)'
#
# and
#
#   'RTEMS_VERSION_ATLEAST(major,minor,revision)'
#
# in config.h so applications can test for a particular
# RTEMS version and conditionally deal with API changes
# and other stuff.
# Note that sometimes (mostly during x.y.99  pre-release
# phases) such changes happen w/o an associated change in
# revision number ;-(.
#
#dnl TILLAC_RTEMS_VERSTEST
AC_DEFUN([TILLAC_RTEMS_VERSTEST],
	[AH_VERBATIM([RTEMS_VERSION_TEST],
				[
#ifndef RTEMS_VERSION_LATER_THAN
#define RTEMS_VERSION_LATER_THAN(ma,mi,re) \
	(    __RTEMS_MAJOR__  > (ma)	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__  > (mi))	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__ == (mi) && __RTEMS_REVISION__ > (re)) \
    )
#endif
#ifndef RTEMS_VERSION_ATLEAST
#define RTEMS_VERSION_ATLEAST(ma,mi,re) \
	(    __RTEMS_MAJOR__  > (ma)	\
	|| (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__  > (mi))	\
	|| (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__ == (mi) && __RTEMS_REVISION__ >= (re)) \
	)
#endif
	            ]dnl
	)]dnl
)

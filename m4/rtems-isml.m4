# Find out if this is a multilibbed RTEMS installation
#
# Result is exit status, i.e., this macro can e.g., be used
# in a 'if MACRO ; then list; fi' statement.
#
# TILLAC_RTEMS_CPUKIT_MULTILIB
AC_DEFUN([TILLAC_RTEMS_CPUKIT_MULTILIB],
	[AC_REQUIRE([AC_CANONICAL_HOST])
	AC_REQUIRE([TILLAC_RTEMS_OPTIONS])
	test -d ${with_rtems_top}/${host_cpu}-${host_os}/include]dnl
)

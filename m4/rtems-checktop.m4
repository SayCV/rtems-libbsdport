# Verify that the --with-rtems-top option has been given
# and that the directory it specifies has a subdirectory
# ${with_rtems_top}/${host_cpu}-${host_os}
#
# Throw an error if this test fails.
#
# TILLAC_RTEMS_CHECK_TOP
#
AC_DEFUN([TILLAC_RTEMS_CHECK_TOP],
	[AC_REQUIRE([AC_CANONICAL_HOST])
    AC_REQUIRE([TILLAC_RTEMS_OPTIONS])
    if TILLAC_RTEMS_OS_IS_RTEMS ; then
        if test ! "${with_rtems_top+set}" = "set" ; then
            AC_MSG_ERROR([No RTEMS topdir given; use --with-rtems-top option])
        fi
        AC_MSG_CHECKING([Checking RTEMS installation topdir])
        if test ! -d $with_rtems_top/${host_cpu}-${host_os}/ ; then
            AC_MSG_ERROR([RTEMS topdir $with_rtems_top/${host_cpu}-${host_os}/ not found])
        fi
        AC_MSG_RESULT([OK])
    fi
    ]dnl
)

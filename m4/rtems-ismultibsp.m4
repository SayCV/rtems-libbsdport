#
# Check if the 'enable_rtemsbsp' variable lists a single
# or multiple BSPs and set exit status accordingly:
#
# Result is exit status, i.e., this macro can e.g., be used
# in a 'if MACRO ; then list; fi' statement.
#
# true  - if enable_rtembsp lists more than one BSP
# false - otherwise
#
# if TILLAC_RTEMS_CHECK_MULTI_BSPS ; then list ; fi
AC_DEFUN([TILLAC_RTEMS_CHECK_MULTI_BSPS],
	[AC_REQUIRE([TILLAC_RTEMS_CHECK_BSPS])
	( _tillac_rtems_multi_bsps=no
    for _tillac_rtems_bspcand in $enable_rtemsbsp ; do
        if test "$_tillac_rtems_multi_bsps" = "no" ; then
            _tillac_rtems_multi_bsps=maybe
        else
            _tillac_rtems_multi_bsps=yes
        fi
    done
    test "$_tillac_rtems_multi_bsps" = "yes")]dnl
)

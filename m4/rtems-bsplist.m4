# Assemble a list of BSPs in 'enable_rtemsbsp'
#
#  a) if 'enable_rtemsbsp' is not set when this macro is expanded
#     then set it to a (whitespace separated) list of all
#     BSPs found under ${with_rtems_top}/${host_cpu}-${host_os}/
#  b) if 'enable_rtemsbsp' is already set then remove all BSPs
#     from it which are not installed under
#     ${with_rtems_top}/${host_cpu}-${host_os}/
#
# -> After expanding this macro 'enable_rtemsbsp' contains a list
#    of all BSPs that are installed and -- if 'enable_rtemsbsp' was
#    initially set -- which are mentioned in 'enable_rtembsp'.
#
# TILLAC_RTEMS_CHECK_BSPS
#
# NOTE: This macro *modifies* the 'enable_rtemsbsp' variable.
#
AC_DEFUN([TILLAC_RTEMS_CHECK_BSPS],
	[AC_REQUIRE([TILLAC_RTEMS_OPTIONS])
    if test ! "${enable_rtemsbsp+set}" = "set" ; then
        _tillac_rtems_bsplist="`ls $with_rtems_top/${host_cpu}-${host_os}/ | tr '\n\r' '  '`"
	else
		_tillac_rtems_bsplist=$enable_rtemsbsp
	fi
	enable_rtemsbsp=
	AC_MSG_CHECKING([Looking for RTEMS BSPs $_tillac_rtems_bsplist])
	for _tillac_rtems_bspcand in $_tillac_rtems_bsplist ; do
		if test -d $with_rtems_top/${host_cpu}-${host_os}/$_tillac_rtems_bspcand/lib/include ; then
			if test "${enable_rtemsbsp}"xx = xx ; then
				enable_rtemsbsp="$_tillac_rtems_bspcand"
			else
				enable_rtemsbsp="$_tillac_rtems_bspcand $enable_rtemsbsp"
			fi
		fi
	done
	if test "$enable_rtemsbsp"xx = "xx" ; then
		AC_MSG_ERROR("No BSPs found")
	else
		AC_MSG_NOTICE([found \'$enable_rtemsbsp\'])
	fi]dnl
)

# fixup the 'exec-prefix' and 'includedir' options:
#  - if either is given explicitly by the user then do nothing
#  - if user says --enable-std-rtems-installdirs then
#      prefix      -> ${rtems_top} 
#      exec-prefix -> ${prefix}/<cpu>/
#      libdir      -> ${exec-prefix}/<bsp>/lib
#      includedir  -> ${libdir}/include
#
#  - if user says nothing then
#
#      exec-prefix -> ${prefix}/target/ssrlApps/<cpu>/<bsp>/
#      includedir  -> ${exec-prefix}/include
#    
AC_DEFUN([TILLAC_RTEMS_FIXUP_PREFIXES],
[
AC_REQUIRE([TILLAC_RTEMS_OPTIONS])
if TILLAC_RTEMS_OS_IS_RTEMS ; then
if test "${enable_std_rtems_installdirs}" = "yes" ; then
	prefix=${with_rtems_top}
	exec_prefix='${prefix}/${host_cpu}-${host_os}/'
	libdir='${exec_prefix}/'${enable_rtemsbsp}/lib
	if test "$enable_multilib" = "yes" ; then
		includedir='${exec_prefix}/include'
	else
		includedir='${libdir}/include'
	fi
	ac_configure_args="${ac_configure_args} --prefix='${prefix}'"
	ac_configure_args="${ac_configure_args} --exec-prefix='${exec_prefix}'"
	ac_configure_args="${ac_configure_args} --libdir='${libdir}'"
	ac_configure_args="${ac_configure_args} --includedir='${includedir}'"
else
# should be correct also for multilibbed build (rtems_bsp empty)
	if test "${exec_prefix}" = "NONE" ; then
		exec_prefix='${prefix}/target/ssrlApps/${host_cpu}-${host_os}/'${enable_rtemsbsp}/
		ac_configure_args="${ac_configure_args} --exec-prefix='${exec_prefix}'"
	fi
	# Unfortunately we have no way to check if includedir was set by the user
	# other than scanning the argument line :-(
	tillac_rtems_includedir_set=no
	for tillac_rtems_arg in ${ac_configure_args} ; do
	case $tillac_rtems_arg in
		-includedir | --includedir | --includedi | --included | --include \
		| --includ | --inclu | --incl | --inc \
        | -includedir=* | --includedir=* | --includedi=* | --included=* | --include=* \
	    | --includ=* | --inclu=* | --incl=* | --inc=*)
		tillac_rtems_includedir_set=yes;
		;;
	*)
	    ;;
	esac
	done

	if test "${tillac_rtems_includedir_set}" = "no" ; then
		includedir='${exec_prefix}/include'
		ac_configure_args="${ac_configure_args} --includedir='${includedir}'"
	fi
fi
fi]dnl
)

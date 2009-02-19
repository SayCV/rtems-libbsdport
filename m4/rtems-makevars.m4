# This macro saves the BSP-specific variables (such as CC, CXX, CPP, ...)
# into intermediate variables from where they can be restored
# with TILLAC_RTEMS_RESET_MAKEVARS
#
# Note: this macro should not be used directly. It is expanded from
#       TILLAC_RTEMS_SETUP
#
# TILLAC_RTEMS_SAVE_MAKEVARS
AC_DEFUN([TILLAC_RTEMS_SAVE_MAKEVARS],
	[
	tillac_rtems_cc_orig="$CC"
	tillac_rtems_cxx_orig="$CXX"
	tillac_rtems_ccas_orig="$CCAS"
	tillac_rtems_cpp_orig="$CPP"
	tillac_rtems_ldflags_orig="$LDFLAGS"
	tillac_rtems_bsp_family_orig=""
	tillac_rtems_bsp_insttop_orig=""]dnl
)

# This macro restores the BSP-specific variables (such as CC, CXX, CPP, ...)
# from intermediate variables that were set by TILLAC_RTEMS_SAVE_MAKEVARS
#
# Note: this macro should not be used directly. It is expanded from
#       TILLAC_RTEMS_SETUP
#
# TILLAC_RTEMS_RESET_MAKEVARS
AC_DEFUN([TILLAC_RTEMS_RESET_MAKEVARS],
	[
	RTEMS_TILL_MAKEVARS_SET=NO
	CC="$tillac_rtems_cc_orig"
	CXX="$tillac_rtems_cxx_orig"
	CCAS="$tillac_rtems_ccas_orig"
	CPP="$tillac_rtems_cpp_orig"
	LDFLAGS="$tillac_rtems_ldflags_orig"
	RTEMS_BSP_FAMILY="$tillac_rtems_family_orig"
	RTEMS_BSP_INSTTOP="$tillac_rtems_insttop_orig"]dnl
)

# Determine critical, BSP-specific build parameters from the
# RTEMS makefiles. These parameters are cflags, gccspecs, ...
#
# Note: this macro should not be used directly. It is expanded from
#       TILLAC_RTEMS_SETUP
#
# TILLAC_RTEMS_MAKEVARS(HOST_SYSTEM, BSP)
AC_DEFUN([TILLAC_RTEMS_MAKEVARS],
	[
	AC_MSG_CHECKING([Determining RTEMS Makefile parameters for BSP:])
dnl DOWNEXT is set in leaf.cfg and we don't include that
	if _tillac_rtems_result=`make -s -f - rtems_makevars <<EOF_
include $with_rtems_top/$1/$2/Makefile.inc
include \\\$(RTEMS_CUSTOM)
include \\\$(CONFIG.CC)

rtems_makevars:
	@echo tillac_rtems_cpu_cflags=\'\\\$(CPU_CFLAGS) \\\$(AM_CFLAGS)\'
	@echo tillac_rtems_gccspecs=\'\\\$(GCCSPECS)\'
	@echo tillac_rtems_cpu_asflags=\'\\\$(CPU_ASFLAGS)\'
	@echo tillac_rtems_ldflags=\'\\\$(AM_LDFLAGS) \\\$(LDFLAGS)\'
	@echo tillac_rtems_cppflags=
	@echo RTEMS_BSP_FAMILY=\'\\\$(RTEMS_BSP_FAMILY)\'
	@echo RTEMS_BSP_INSTTOP=\'\\\$(PROJECT_RELEASE)\'
EOF_
` ; then
	AC_MSG_RESULT([OK: $_tillac_rtems_result])
	else
	AC_MSG_ERROR([$_tillac_rtems_result])
	fi
	# propagate cpu_cflags and gccspecs into currently executing shell
	eval $_tillac_rtems_result
	export RTEMS_BSP_INSTTOP="$RTEMS_BSP_INSTTOP"
	export RTEMS_BSP_FAMILY="$RTEMS_BSP_FAMILY"]dnl
)

# Export the set of critical, BSP-specific build parameters 
# (cflags, gccspecs, ...) that were determined by TILLAC_RTEMS_MAKEVARS
# into the environment.
#
# This macro takes two (optional) arguments: 
#
#    <HOST_SYSTEM> and <LIBSUBDIR>
#
# The macro also, adds the paths listed in the --with-extra-incdirs and
# --with-extra-libdirs options to the cppflags and ldflags, respectively
# (adding -I, -L). If 'LIBSUBDIR' is given then it is appended to
# any of the directories listed in --with-extra-libdirs (if the LIBSUBDIR
# exists) there.
#
# Furthermore, if it is determined that RTEMS was configured for
# a multilibbed cpukit then
#  i)   -I${with_rtems_top}/${host_cpu}-${host_os}/include 
#       is added to the cppflags (if the directory exists)
#  ii)  -B${with_rtems_top}/<HOST_SYSTEM>/lib is added to the
#       gcc specs so that multilibs are found (workaround for
#       a bug in the rtems 4.9.0 makefiles)
#
# Note: this macro should not be used directly. It is expanded from
#       TILLAC_RTEMS_SETUP
#
# TILLAC_RTEMS_EXPORT_MAKEVARS([HOST_SYSTEM],[LIBSUBDIR])
AC_DEFUN([TILLAC_RTEMS_EXPORT_MAKEVARS],
	[
	AC_MSG_CHECKING([Checking if RTEMS CC & friends MAKEVARS are already set])
	if test ! "${RTEMS_TILL_MAKEVARS_SET}" = "YES"; then
		AC_MSG_RESULT([No (probably a multilibbed build)]) 
		export RTEMS_TILL_MAKEVARS_SET=YES
		# if this is a multilibbed cpukit we need to include
		if test -d $with_rtems_top/${host_cpu}-${host_os}/include ; then
			tillac_rtems_cppflags="$tillac_rtems_cppflags -I$with_rtems_top/${host_cpu}-${host_os}/include"
			# and since the RTEMS (4.9) makefiles seem to be broken
			# for multilibbed cpukits (fail to add -B <libdir>) we
			# do it here
			tillac_rtems_gccspecs="$tillac_rtems_gccspecs -B $with_rtems_top/$1/lib"
		fi
		if test "${with_extra_incdirs+set}" = "set" ; then
			for tillac_extra_incs_val in ${with_extra_incdirs} ; do
				tillac_rtems_cppflags="$tillac_rtems_cppflags -I$tillac_extra_incs_val"
			done
		fi
		if test "${with_extra_libdirs+set}" = "set" ; then
			for tillac_extra_libs_val in ${with_extra_libdirs} ; do
				if test -d $tillac_extra_libs_val/$2 ; then
					tillac_rtems_ldflags="$tillac_rtems_ldflags -L$tillac_extra_libs_val/$2"
				else
					tillac_rtems_ldflags="$tillac_rtems_ldflags -L$tillac_extra_libs_val"
				fi
			done
		fi
#export forged CC & friends so that they are used by sub-configures, too
		export CC="$CC $tillac_rtems_gccspecs $tillac_rtems_cpu_cflags $tillac_rtems_cppflags"
		export CXX="$CXX $tillac_rtems_gccspecs $tillac_rtems_cpu_cflags $tillac_rtems_cppflags"
		export CCAS="$CCAS $tillac_rtems_gccspecs $tillac_rtems_cpu_asflags -DASM"
		export CPP="$CPP $tillac_rtems_gccspecs $tillac_rtems_cppflags"
#		export CFLAGS="$CFLAGS $tillac_rtems_cpu_cflags"
#		export CXXFLAGS="$CXXFLAGS  $tillac_rtems_cpu_cflags"
#		export CCASFLAGS="$CCASFLAGS $tillac_rtems_cpu_asflags -DASM"
#		export CPPFLAGS="$CPPFLAGS $tillac_rtems_cppflags"
		export LDFLAGS="$LDFLAGS $tillac_rtems_ldflags"
	else
		AC_MSG_RESULT([yes])
	fi]dnl
)

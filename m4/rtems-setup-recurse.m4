# This macro assembles a list of RTEMS CPUs and if the list
# contains more than one member then it creates a build subdirectory
# for each CPU architecture, 'chdirs' into the subdirectory and recursively
# configures for the CPU.
#
# Several options affect the behavior of this macro:
#
#   --with-rtems-top
#   --host
#   --enable-rtemsbsp
#
#  1) if --with-rtems-top is NOT given (NOT RTEMS) OR if
#     --host=<xyz>-rtems was given (RTEMS CPU defined by user)
#     THEN the macro does nothing.
#
#  2) ELSE (--with-rtems-top given but --host is NOT *rtems* 
#     i)   assemble a list of all '<cpu>-rtems*' subdirectories
#          under ${with_rtems_top}
#
#     ii)  if --enable-rtemsbsp was given then remove CPU
#          architectures not being required by any of the listed
#          BSPS.
#
#     iii) for each remaining CPU architecture create a subdirectory,
#          chdir there and recursively call 'configure' again with
#          the original arguments but --host=<cpu>-rtems appended.
#
# Note: this macro should not be used directly. It is expanded from
#       TILLAC_RTEMS_SETUP
#
# TILLAC_RTEMS_CONFIG_CPUS_RECURSIVE
AC_DEFUN([TILLAC_RTEMS_CONFIG_CPUS_RECURSIVE],
	[if test ! "${with_rtems_top+set}" = "set" || TILLAC_RTEMS_HOSTOS_IS_RTEMS ; then : ; else
	# with_rtems_top is set but host_os is not *rtems*, i.e.,
	# we have to figure out a list of CPUs/arches that are installed.
	AC_MSG_CHECKING([for all installed CPUs/architectures])
	_tillac_rtems_cpulist="`(cd $with_rtems_top; ls -d *-rtems* | tr '\n\r' '  ')`"
	AC_MSG_RESULT([Found: $_tillac_rtems_cpulist])
	# if 'enable-rtemsbsp' was given then filter away
	# architectures that match none of the BSPs
	if test "${enable_rtemsbsp+set}" = "set" ; then
		# convert space separated list into ORed (|) pattern
		_tillac_rtems_bspfilt=`echo "$enable_rtemsbsp" | sed -e 's/[[ \t]]\+/|/g'`
		AC_MSG_NOTICE([Filtering CPU/architecture list against bsps: $_tillac_rtems_bspfilt])
		_tillac_rtems_cpuall="$_tillac_rtems_cpulist"
		_tillac_rtems_cpulist=""
		AC_MSG_CHECKING([CPU/architectures matching requested BSPs])
		for _tillac_rtems_cpucand in $_tillac_rtems_cpuall ; do
			# look for directories which have a 'Makefile.inc'
			for _tillac_rtems_bspcand in `(cd $with_rtems_top/$_tillac_rtems_cpucand ; ls */Makefile.inc | tr '\n\r' '  ')` ; do
				AC_MSG_NOTICE([testing $_tillac_rtems_bspcand])
				# reduce to bsp name
				_tillac_rtems_bspcand=`dirname $_tillac_rtems_bspcand`
				if eval "case `echo $_tillac_rtems_bspcand` in $_tillac_rtems_bspfilt) : ;; *) false ;; esac" ; then
					# only add candidate to list of cpus if not already there
					if test -z "$_tillac_rtems_cpulist" ; then
						_tillac_rtems_cpulist="$_tillac_rtems_cpucand"
					else
						_tillac_rtems_cpufilt=`echo "$_tillac_rtems_cpulist" | sed -e 's/[[ \t]]\+/|/g'`
						if eval "case `echo $_tillac_rtems_cpucand` in $_tillac_rtems_cpufilt) false ;; *) : ;; esac" ; then
							_tillac_rtems_cpulist="$_tillac_rtems_cpulist $_tillac_rtems_cpucand"
						fi
					fi
				fi
			done
		done
		AC_MSG_RESULT([found: $_tillac_rtems_cpulist])
	fi
	if test "$_tillac_rtems_cpulist"xx = "xx" ; then
		AC_MSG_ERROR([No RTEMS architectures found])
	fi
	# Create directory and configure
	for _tillac_rtems_cpucand in $_tillac_rtems_cpulist ; do
		if test -d $_tillac_rtems_cpucand || mkdir $_tillac_rtems_cpucand ; then : ; else
			AC_MSG_ERROR([Unable to create subdirectory $_tillac_rtems_cpucand])
		fi
		TILLAC_RTEMS_TRIM_CONFIG_DIR(_tillac_rtems_config_dir)
		# SUB-CONFIGURE
		AC_MSG_NOTICE([Running $_tillac_rtems_config_dir/[$]0 "$ac_configure_args --host=$_tillac_rtems_cpucand" in "'$_tillac_rtems_cpucand'" subdir])
		eval \( cd $_tillac_rtems_cpucand \; $SHELL $_tillac_rtems_config_dir/"[$]0" $ac_configure_args --host=$_tillac_rtems_cpucand \)
	done
	AC_MSG_NOTICE([Creating cpu/arch level makefile])
    AC_SUBST(the_subdirs,[$_tillac_rtems_cpulist])
	AC_SUBST(the_distsub,['$(firstword '"$_tillac_rtems_cpulist"')'])
	_tillac_rtems_recursing=yes
	false
	fi]dnl
)

# This macro
#
#  - removes --enable-rtemsbsp options from the current commandline
#  - for each BSP listed in '${enable_rtemsbsp}'
#     i)   creates a BSP subdirectory in the build tree
#     ii)  chdirs into the subdirectory
#     iii) figures out a few BSP-specific build settings
#          (cflags, gcc specs, ...)
#     iv)  configures for the BSP passing the properties determined
#          under iii) along to 'configure' on the commandline.
#          Also, --enable-rtemsbsp=<this_bsp> is appended to the 
#          configure commandline.
#
# Note: this macro should not be used directly. It is expanded from
#       TILLAC_RTEMS_SETUP
#
# TILLAC_RTEMS_CONFIG_BSPS_RECURSIVE
AC_DEFUN([TILLAC_RTEMS_CONFIG_BSPS_RECURSIVE],
	[if test ! "${RTEMS_TILL_MAKEVARS_SET}" = "YES"; then
		# strip all --enable-rtemsbsp options from original
		# commandline
		AC_MSG_NOTICE([Stripping --enable-rtemsbsp option(s) from commandline])
		_tillac_rtems_config_args=""
	    eval for _tillac_rtems_arg in $ac_configure_args \; do case \$_tillac_rtems_arg in --enable-rtemsbsp\* \) \;\; \*\) _tillac_rtems_config_args=\"\$_tillac_rtems_config_args \'\$_tillac_rtems_arg\'\" \;\; esac done
		AC_MSG_NOTICE([Commandline now: $_tillac_rtems_config_args])
			
		AC_MSG_NOTICE([Creating BSP subdirectories and sub-configuring])
		TILLAC_RTEMS_SAVE_MAKEVARS
		for _tillac_rtems_bsp in $enable_rtemsbsp ; do
			if test ! -d $_tillac_rtems_bsp ; then
				AC_MSG_CHECKING([Creating $_tillac_rtems_bsp])
				if mkdir $_tillac_rtems_bsp ; then
					AC_MSG_RESULT([OK])
				else
					AC_MSG_ERROR([Failed])
				fi
			fi
			TILLAC_RTEMS_TRIM_CONFIG_DIR(_tillac_rtems_config_dir)
			TILLAC_RTEMS_RESET_MAKEVARS
			TILLAC_RTEMS_MAKEVARS(${host_cpu}-${host_os},$_tillac_rtems_bsp)
			tillac_rtems_cppflags="$tillac_rtems_cppflags -I$with_rtems_top/${host_cpu}-${host_os}/$_tillac_rtems_bsp/lib/include"
			TILLAC_RTEMS_EXPORT_MAKEVARS(${host_cpu}-${host_os},$_tillac_rtems_bsp)
			AC_MSG_NOTICE([Running $_tillac_rtems_config_dir/[$]0 $_tillac_rtems_config_args --enable-rtemsbsp=$_tillac_rtems_bsp in $_tillac_rtems_bsp subdir])
			eval \( cd $_tillac_rtems_bsp \; $SHELL $_tillac_rtems_config_dir/"[$]0" $_tillac_rtems_config_args --enable-rtemsbsp=$_tillac_rtems_bsp \)
		done
		TILLAC_RTEMS_RESET_MAKEVARS
		AC_MSG_NOTICE([Creating toplevel makefile])
	    AC_SUBST(the_subdirs,[$enable_rtemsbsp])
		AC_SUBST(the_distsub,['$(firstword '"$enable_rtemsbsp"')'])
	fi]dnl
)

# Grand Master Macro for RTEMS configuration.
#
# This sets up most things for a RTEMS configuration
# for multiple CPU-arches and BSPs.
#
# A package may add the optional (literal) argument
# 'domultilib'. In this case, multilib support is enabled
# and the user may configure with '--enable-multilib'.
# Note that the package must properly support multilibs!
#
# If the host system is not RTEMS (no with-rtems-top given)
# then this macro does *nothing*.
# 
# TILLAC_RTEMS_SETUP([domultilib])
AC_DEFUN([TILLAC_RTEMS_SETUP],
    [AC_REQUIRE([TILLAC_RTEMS_OPTIONS])dnl
	AM_CONDITIONAL(OS_IS_RTEMS,[TILLAC_RTEMS_OS_IS_RTEMS])
	if TILLAC_RTEMS_CONFIG_CPUS_RECURSIVE ; then
	m4_if($1,domultilib,
		[TILLAC_RTEMS_MULTILIB([Makefile],[.])],
		[AC_REQUIRE([TILLAC_RTEMS_OPTIONS])dnl
		if test "${enable_multilib}" = "yes" ; then
		 	AC_MSG_ERROR(["multilibs not supported, sorry"])
		fi]dnl
	)
	if TILLAC_RTEMS_OS_IS_RTEMS ; then
		TILLAC_RTEMS_CHECK_TOP
		AC_ARG_VAR([RTEMS_TILL_MAKEVARS_SET],[Internal use; do NOT set in environment nor on commandline])
		AC_ARG_VAR([DOWNEXT],[extension of downloadable binary (if applicable)])
		AC_ARG_VAR([APPEXEEXT], [extension of linked binary])
		AC_ARG_VAR([RTEMS_BSP_FAMILY],[Internal use; do NOT set in environment nor on commandline])
		AC_ARG_VAR([RTEMS_BSP_INSTTOP],[Internal use; do NOT set in environment nor on commandline])
		if test "$1" = "domultilib" && test "$enable_multilib" = "yes" ; then 
			if test "${enable_rtemsbsp+set}" = "set" ; then
				AC_MSG_ERROR([Cannot --enable-rtemsbsp AND --enable-multilib; build either multilibs or for particular BSP(s)])
			fi
			TILLAC_RTEMS_EXPORT_MAKEVARS(${host_cpu}-${host_os},)
		else
			TILLAC_RTEMS_CHECK_BSPS
		fi
		if test ! "${RTEMS_TILL_MAKEVARS_SET}" = "YES"; then
			TILLAC_RTEMS_CONFIG_BSPS_RECURSIVE(makefile)
			_tillac_rtems_recursing=yes
		else
			TILLAC_RTEMS_FIXUP_PREFIXES
dnl set those in the configure script so that 'configure' uses these settings when trying to compile stuff
dnl		AC_SUBST(rtems_gccspecs,   [$tillac_rtems_gccspecs])
dnl		AC_SUBST(rtems_cpu_cflags, [$tillac_rtems_cpu_cflags])
dnl		AC_SUBST(rtems_cpu_asflags,["$tillac_rtems_cpu_asflags -DASM"])
dnl		AC_SUBST(rtems_cppflags,   [$tillac_rtems_cppflags])
			AC_SUBST(rtems_bsp,        [$enable_rtemsbsp])
			AC_MSG_NOTICE([Setting DOWNEXT to .ralf])
			DOWNEXT=.ralf
			AC_MSG_NOTICE([Setting APPEXEEXT to .exe])
			APPEXEEXT=.exe
			TILLAC_RTEMS_VERSTEST
		fi
	fi
	fi
	if test "${_tillac_rtems_recursing}" = "yes" ; then
		AC_CONFIG_FILES([makefile:makefile.top.in])
		AC_OUTPUT
		exit 0
		false
	fi]dnl
)

dnl m4_syscmd is executed when aclocal is run
m4_syscmd([cat - > makefile.top.am <<'EOF_'
AUTOMAKE_OPTIONS=foreign
SUBDIRS=@the_subdirs@
# When making a distribution we only want to 
# recurse into (any) one single BSP subdir.
DIST_SUBDIRS=@the_distsub@

# The dist-hook then removes this extra
# directory level again.
dist-hook:
	if test "$(PACKAGE_VERSION)" = "untagged" ; then echo "Need tagged version to cut distribution"; exit 1; fi
	cp -frl $(distdir)/$(DIST_SUBDIRS)/* $(distdir)
	rm -fr  $(distdir)/$(DIST_SUBDIRS)
EOF_
])

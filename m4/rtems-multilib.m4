#
# Prepare for a multilibbed build
#  - check for presence of 'config-ml.in'
#  - expand AM_ENABLE_MULTILIB(MAKEFILE, REL-TO-TOP-SRCDIR)
#  - expand TILLAM_MULTISUB_INSTALLDIR (workaround so that
#    multilibs are installed into proper subdir.
#  - make sure  'enable_multilib' is set to 'no' if it was initially
#    unset; yet another little workaround...
#
# TILLAC_RTEMS_MULTILIB([MAKEFILE], [REL-TO-TOP-SRCDIR])
AC_DEFUN([TILLAC_RTEMS_MULTILIB],
	[if test -f ${srcdir}/config-ml.in || test -f $(srcdir)/../config-ml.in ; then
		AM_ENABLE_MULTILIB([$1],[$2])
		# install multilibs into MULTISUBDIR
		TILLAM_MULTISUB_INSTALLDIR
dnl		AC_SUBST(libdir,[${libdir}'$(MULTISUBDIR)'])
		# in order to properly build multilibs in sub-libraries it seems we
		# must pass the --enable-multilibs arg to sub-configures or multilibs
		# are not built there.
		# To work around, we simply set the default to 'no' so the user must
		# say --enable-multilib to get them.
		if test ! "${enable_multilib+set}" = "set" ; then
		    multilib=no
		fi
	else
		enable_multilib=no
	fi]dnl
)

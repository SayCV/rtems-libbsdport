# TILLAM_MULTISUB_INSTALLDIR
#
# tweak 'libdir' so that libraries are
# installed in proper multisubdir.
#
# For use by 'sub-packages', i.e., from
# configure.ac in a subdir of a main
# package. Only the toplevel configure.ac
# should say AM_ENABLE_MULTILIB
#
AC_DEFUN([TILLAM_MULTISUB_INSTALLDIR],
[# Install multilib into proper multisubdir
if test "${with_multisubdir+set}" = "set" ; then
  the_multisubdir="/${with_multisubdir}"
else
  the_multisubdir=
fi
AC_SUBST(libdir,[${libdir}${the_multisubdir}])])dnl


])dnl

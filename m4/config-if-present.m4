dnl while we can get autoconf to properly handle nonexisting
dnl directories (e.g., because a sub-package only requires and
dnl distributes a subset of the directories under ssrlApps)
dnl via 
dnl    if [ -d somedir ] ; then AC_CONFIG_FILES([somedir/Makefile]) ; fi
dnl this doesn't work for automake -- automake doesn't understand
dnl the shell commands.
dnl
dnl The 'TILLAC_M4_IF_PRESENT' macro tests at 'autoconf/automake'-time
dnl if the argument file with '.am' appended exists and expands
dnl $2 if it does. Otherwise, $2 is suppressed.
dnl
dnl USAGE:
dnl 
dnl TILLAC_M4_IR_PRESENT(
dnl    [mysubdir/Makefile],
dnl    [<configure commands to configure 'mysubdir'
dnl      including AC_CONFIG_FILES([mysubdir/Makefile])])
dnl 
AC_DEFUN([TILLAC_M4_IF_PRESENT],
[m4_syscmd([test -d $1])
m4_if(0,m4_sysval,[$2])]dnl
)

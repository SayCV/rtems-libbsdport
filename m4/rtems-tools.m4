# Check for critical programs we need for building
AC_DEFUN([TILLAC_RTEMS_CHECK_TOOLS],
	[AC_PROG_CC
	 AM_PROG_AS
	 AC_PROG_CXX
	 AC_SUBST([GCC])
	 AC_PROG_CPP
	 AC_CHECK_PROGS([HOSTCC], gcc cc)
	 RTEMS_CHECK_TOOL([AR],ar)
	 RTEMS_CHECK_TOOL([LD],ld)
	 RTEMS_CHECK_TOOL([OBJCOPY],objcopy)
	 RTEMS_CHECK_TOOL([RANLIB],ranlib)
	 AC_PROG_INSTALL
	 AC_CHECK_PROG([INSTALL_IF_CHANGE],[install-if-change],[install-if-change],[${INSTALL}])]dnl
)

## Check for a program, similar to AC_CHECK_PROG, but lets
## configure fail if the program is not found
#dnl RTEMS_CHECK_PROG(VARIABLE, PROG-TO-CHECK-FOR, VALUE-IF-FOUND [, VALUE-IF-NOT-FOUND [, PATH [, REJECT]]])
AC_DEFUN([RTEMS_CHECK_PROG],
[
	AC_CHECK_PROG($1,$2,$3,$4,$5,$6)
	AS_IF([test -z "${$1}"],
		[AC_MSG_ERROR([program '$2' not found.])])
])

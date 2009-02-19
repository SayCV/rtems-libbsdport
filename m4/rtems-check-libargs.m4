# This macro can be provided as a 5th argument
# to AC_CHECK_LIB() so that linking an RTEMS
# application works. Without that, linking would
# fail because the application usually supplies
# rtems_bsdnet_config.
# In order to link, we create a dummy symbol.
AC_DEFUN([TILLAC_RTEMS_CHECK_LIB_ARGS],
	[[-Wl,--defsym,rtems_bsdnet_config=0]]dnl
)

# Takes one argument
#
# TILLAC_RTEMS_TRIM_CONFIG_DIR(dirvar)
#
# If 'srcdir' is a absolute path (a string
# starting with '/' then set 'dirvar' to the
# empty string; otherwise (srcdir is a relative path)
# set 'dirvar' to '../'.
#
# This macro can be used to find 'srcdir' should 
# configure decide to step into a subdirectory inside
# a build tree.
#
AC_DEFUN([TILLAC_RTEMS_TRIM_CONFIG_DIR],
	[AC_MSG_NOTICE([Trimming source directory])
	# leave absolute path alone, relative path needs
	# to step one level up
	case $srcdir in
		/* )
			$1=
		;;
		*)
			$1=../
		;;
	esac]dnl
)

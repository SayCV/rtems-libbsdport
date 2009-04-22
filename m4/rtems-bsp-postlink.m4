# Define 'postlink' commands based on BSP family
#
# NOTE: This is NOT extracted from the RTEMS makefiles but
#       essentially a copy of what rtems-4.9.0 does.
#       It would be too hard to figure this one out ;-(
#
AC_DEFUN([TILLAC_RTEMS_BSP_POSTLINK_CMDS],
	[AC_ARG_VAR([RTEMS_BSP_POSTLINK_CMDS],[Command sequence to convert ELF file into downloadable executable])
	AC_MSG_NOTICE([Setting RTEMS_BSP_POSTLINK_CMDS based on RTEMS_BSP_FAMILY])
	case "$RTEMS_BSP_FAMILY" in
		svgm|beatnik|mvme5500|mvme3100|uC5282|mvme167|mvme162)
# convert ELF -> pure binary
			RTEMS_BSP_POSTLINK_CMDS='$(OBJCOPY) -Obinary -R .comment -S $(basename $[@])$(APPEXEEXT) $[@]'
		;;
		motorola_powerpc)
# convert ELF -> special PREP bootloader
			RTEMS_BSP_POSTLINK_CMDS=\
'$(OBJCOPY) -O binary -R .comment -S $(basename $[@])$(APPEXEEXT) rtems ;'\
'gzip -vf9 rtems ; '\
'$(LD) -o $(basename $[@])$(DOWNEXT)  $(RTEMS_BSP_INSTTOP)/lib/bootloader.o '\
'--just-symbols=$(basename $[@])$(APPEXEEXT) '\
'-b binary rtems.gz -T $(RTEMS_BSP_INSTTOP)/lib/ppcboot.lds '\
'-Map $(basename $[@]).map && chmod 755 $(basename $[@])$(DOWNEXT) ; '\
'rm -f rtems.gz'
		;;
# default: empty command
		*)
		;;
	esac
	AC_MSG_NOTICE([RTEMS_BSP_POSTLINK_CMDS: "$RTEMS_BSP_POSTLINK_CMDS"])
	AM_CONDITIONAL([HAVE_BSP_POSTLINK_CMDS], [test ! "$RTEMS_BSP_POSTLINK_CMDS"xx = "xx" ])]dnl
)

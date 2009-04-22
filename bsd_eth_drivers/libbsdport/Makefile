#
#  Makefile.lib,v 1.5 2000/06/12 15:00:14 joel Exp
#
# Templates/Makefile.lib
#       Template library Makefile
#

LIBNAME=libbsdport.a
LIB=${ARCH}/${LIBNAME}

# C and C++ source names, if any, go here -- minus the .c or .cc
C_PIECES=rtems_callout rtems_taskqueue rtems_udelay ifstuff devicet alldrv
C_PIECES+=contigmalloc sysbus malloc ifmedia
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

CC_PIECES=
CC_FILES=$(CC_PIECES:%=%.cc)
CC_O_FILES=$(CC_PIECES:%=${ARCH}/%.o)

H_FILES=libbsdport_api.h rtems_verscheck.h

LINKS+=sys/taskqueue.h
LINKS+=sys/bus.h
LINKS+=sys/mutex.h
LINKS+=bsp/rtems_verscheck.h

DUMMYHEADERS+=dummyheaders/machine/bus.h
DUMMYHEADERS+=dummyheaders/machine/resource.h
DUMMYHEADERS+=dummyheaders/dev/pci/pcivar.h
DUMMYHEADERS+=dummyheaders/dev/pci/pcireg.h
DUMMYHEADERS+=dummyheaders/dev/mii/miivar.h
DUMMYHEADERS+=dummyheaders/sys/module.h
DUMMYHEADERS+=dummyheaders/sys/rman.h
DUMMYHEADERS+=dummyheaders/sys/kthread.h
DUMMYHEADERS+=dummyheaders/sys/endian.h
DUMMYHEADERS+=dummyheaders/net/if_vlan_var.h
DUMMYHEADERS+=dummyheaders/netinet/ip6.h
DUMMYHEADERS+=dummyheaders/vm/pmap.h
DUMMYHEADERS+=dummyheaders/miibus_if.h

# Assembly source names, if any, go here -- minus the .S
S_PIECES=
S_FILES=$(S_PIECES:%=%.S)
S_O_FILES=$(S_FILES:%.S=${ARCH}/%.o)

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

all:

depend: ${LINKS}

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc

include $(RTEMS_CUSTOM)
include $(RTEMS_ROOT)/make/lib.cfg

#
# Add local stuff here using +=
#

DEFINES  +=
CPPFLAGS += -I.
CFLAGS   +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += 
CLOBBER_ADDITIONS += dummyheaders
CLOBBER_ADDITIONS += $(sort $(foreach n,$(LINKS),$(firstword $(subst /, ,$(n)))))

all: ${DUMMYHEADERS} ${LINKS} ${ARCH} $(SRCS) $(LIB)

$(LIB): ${OBJS}
	$(make-library)

dummyheaders/%:
	@if [ ! -d `dirname $@` ] ; then mkdir -p `dirname $@`; fi
	@touch $@

# for each name listed in LINKS, create parent directories (if needed)
# and a symlink to file in .
# E.g., LINKS=a/b/c.h
# creates a/b/c.h -> ../../c.h
$(LINKS):
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@ln -s `echo $@ | sed -e 's%[^/]\+[/]\+%../%g'` $@

ifndef RTEMS_SITE_INSTALLDIR
RTEMS_SITE_INSTALLDIR = $(PROJECT_RELEASE)
endif

ifndef RTEMS_SITE_BSP_INSTALLDIR
RTEMS_SITE_BSP_INSTALLDIR = $(RTEMS_SITE_INSTALLDIR)
endif

${RTEMS_SITE_INSTALLDIR}/include/bsp \
${RTEMS_SITE_INSTALLDIR}/lib \
${RTEMS_SITE_INSTALLDIR}/bin:
	test -d $@ || mkdir -p $@

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL_CHANGE)
#

install:  all $(RTEMS_SITE_INSTALLDIR)/lib $(RTEMS_SITE_INSTALLDIR)/include/bsp
	$(INSTALL_VARIANT) -m 644 ${LIB} ${RTEMS_SITE_INSTALLDIR}/lib
	$(INSTALL_CHANGE)  -m 644 ${H_FILES} ${RTEMS_SITE_INSTALLDIR}/include/bsp



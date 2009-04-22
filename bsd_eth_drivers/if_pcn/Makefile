#
#  Makefile.leaf,v 1.7 2002/07/22 22:56:09 joel Exp
#
# Templates/Makefile.leaf
# 	Template leaf node Makefile
#
#
LIBNAME=libif_pcn.a

LINKS=pci/if_pcnreg.h

# C source names, if any, go here -- minus the .c
C_PIECES=

C_PIECES+=if_pcn

C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

# C++ source names, if any, go here -- minus the .cc
CC_PIECES=
CC_FILES=$(CC_PIECES:%=%.cc)
CC_O_FILES=$(CC_PIECES:%=${ARCH}/%.o)

H_FILES=

# Assembly source names, if any, go here -- minus the .S
S_PIECES=
S_FILES=$(S_PIECES:%=%.S)
S_O_FILES=$(S_FILES:%.S=${ARCH}/%.o)

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

# If your PGMS target has the '.exe' extension, a statically
# linked application is generated.
# If it has a '.obj' extension, a loadable module is built.
#
#
ifdef LIBNAME
LIB=${ARCH}/${LIBNAME}
else
PGMS=${ARCH}/if_pcn.obj
endif

#  List of RTEMS Classic API Managers to be included in the application
#  goes here. Use:
#     MANAGERS=all
# to include all RTEMS Classic API Managers in the application or
# something like this to include a specific set of managers.
#     MANAGERS=io event message rate_monotonic semaphore timer
#
# UNUSED for loadable modules
MANAGERS=ALL

ifndef RTEMS_MAKEFILE_PATH
$(error you need to set the RTEMS_MAKEFILE_PATH environment variable)
endif

all:

depend: ${LINKS}

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc

include $(RTEMS_CUSTOM)
ifdef LIBNAME
include $(RTEMS_ROOT)/make/lib.cfg
else
include $(RTEMS_ROOT)/make/leaf.cfg
endif

#
# (OPTIONAL) Add local stuff here using +=
#

#DEFINES  += -DPCN_DEBUG
CPPFLAGS += -I. -I../libbsdport -I../libbsdport/dummyheaders
CFLAGS   +=

#
# CFLAGS_DEBUG_V are used when the `make debug' target is built.
# To link your application with the non-optimized RTEMS routines,
# uncomment the following line:
# CFLAGS_DEBUG_V += -qrtems_debug
#

LD_PATHS  += 
LD_LIBS   += 
LDFLAGS   +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += 
CLOBBER_ADDITIONS += pci

all: ${LINKS} ${ARCH} $(SRCS) $(PGMS) ${LIB}

pci/%:%
	if [ ! -d pci ]; then mkdir -p pci; fi ; ln -s ../$^ $@

#How to make a relocatable object
$(filter %.obj, $(PGMS)): ${OBJS}
	$(make-obj)

#How to make an executable (statically linked)
$(filter %.exe,$(PGMS)): ${LINK_FILES}
	$(make-exe)
ifdef ELFEXT
ifdef XSYMS
	$(XSYMS) $(@:%.exe=%.$(ELFEXT)) $(@:%.exe=%.sym)
endif
endif

$(LIB): ${OBJS}
	$(make-library)

ifndef RTEMS_SITE_INSTALLDIR
RTEMS_SITE_INSTALLDIR = $(PROJECT_RELEASE)
endif

${RTEMS_SITE_INSTALLDIR}/include \
${RTEMS_SITE_INSTALLDIR}/lib \
${RTEMS_SITE_INSTALLDIR}/bin:
	test -d $@ || mkdir -p $@
# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL_CHANGE)
#
#  - Some BSPs might generate bootable executables in yet another
#    format (such as .srec) and you might need to extend the rule
#    below so the essential files get installed. YMMV.
ifdef LIBNAME
install:  all $(RTEMS_SITE_INSTALLDIR)/lib
	$(INSTALL_VARIANT) -m 644 ${LIB} ${RTEMS_SITE_INSTALLDIR}/lib
else
install:  all $(RTEMS_SITE_INSTALLDIR)/bin
	$(INSTALL_VARIANT) -m 555 ${PGMS} ${PGMS:%.exe=%.bin} ${PGMS:%.exe=%.sym} ${RTEMS_SITE_INSTALLDIR}/bin
endif

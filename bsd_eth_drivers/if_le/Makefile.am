#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libif_le_a_SOURCES     = lance.c am79900.c if_le_pci.c

libif_le_a_SOURCES    += am79900reg.h am79900var.h am7990reg.h
libif_le_a_SOURCES    += am7990var.h lancereg.h lancevar.h

lib_LIBRARIES          = libif_le.a


AM_CPPFLAGS           += -I$(srcdir)
AM_CPPFLAGS           += -I$(srcdir)/../libbsdport -I../libbsdport -I../libbsdport/dummyheaders
#AM_CPPFLAGS           += -DLEDEBUG

include $(top_srcdir)/rtems.am

LINKS                  =
LINKS                 +=dev/le/lancereg.h
LINKS                 +=dev/le/lancevar.h
LINKS                 +=dev/le/am79900reg.h
LINKS                 +=dev/le/am79900var.h

BUILT_SOURCES          =

$(libif_le_a_OBJECTS): $(LINKS)

include ../links.am

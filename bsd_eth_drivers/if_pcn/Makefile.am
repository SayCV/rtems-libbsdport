#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libif_pcn_a_SOURCES    = if_pcn.c if_pcnreg.h
lib_LIBRARIES          = libif_pcn.a

AM_CPPFLAGS           +=-I$(srcdir)
AM_CPPFLAGS           += -I$(srcdir)/../libbsdport -I../libbsdport -I../libbsdport/dummyheaders
#AM_CPPFLAGS           += -DPCN_DEBUG

include $(top_srcdir)/rtems.am

LINKS                  = pci/if_pcnreg.h

BUILT_SOURCES          =

$(libif_pcn_a_OBJECTS): $(LINKS)

include ../links.am

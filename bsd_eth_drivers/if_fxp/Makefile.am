#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libif_fxp_a_SOURCES              = if_fxp.c
libif_fxp_a_SOURCES             += if_fxpreg.h  if_fxpvar.h  rcvbundl.h

lib_LIBRARIES                    = libif_fxp.a

AM_CPPFLAGS                     += -I$(srcdir)
AM_CPPFLAGS                     += -I$(srcdir)/../libbsdport -I../libbsdport  -I../libbsdport/dummyheaders
AM_CPPFLAGS                     += $(CPPFLAGS_82542_SUPPORT_$(ENBL_82542_SUPPORT))
AM_CPPFLAGS                     += $(CPPFLAGS_ICH8LAN_SUPPORT_$(ENBL_ICH8LAN_SUPPORT))

LINKS                            = dev/fxp/if_fxpreg.h dev/fxp/if_fxpvar.h dev/fxp/rcvbundl.h

BUILT_SOURCES                    =

$(libif_fxp_a_OBJECTS): $(LINKS)

include ../links.am

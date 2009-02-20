#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libif_bge_a_SOURCES               = if_bge.c if_bgereg.h

##EXTRA_libif_bge_a_SOURCES         = 

CPPFLAGS_82542_SUPPORT_NO        = -DNO_82542_SUPPORT
CPPFLAGS_ICH8LAN_SUPPORT_NO      = -DNO_ICH8LAN_SUPPORT

libif_bge_a_LIBADD                =

libif_bge_a_DEPENDENCIES          = $(libif_bge_a_LIBADD)

lib_LIBRARIES                    = libif_bge.a

AM_CPPFLAGS                     += -I$(srcdir)
AM_CPPFLAGS                     += -I$(srcdir)/../libbsdport -I../libbsdport  -I../libbsdport/dummyheaders

include $(top_srcdir)/rtems.am

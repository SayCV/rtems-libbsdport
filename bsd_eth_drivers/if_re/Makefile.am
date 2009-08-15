#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libif_re_a_SOURCES               = if_re.c if_rlreg.h

##EXTRA_libif_re_a_SOURCES         = 

libif_re_a_LIBADD                =

libif_re_a_DEPENDENCIES          = $(libif_re_a_LIBADD)

lib_LIBRARIES                    = libif_re.a

AM_CPPFLAGS                     += -I$(srcdir)
AM_CPPFLAGS                     += -I$(srcdir)/../libbsdport -I../libbsdport  -I../libbsdport/dummyheaders

include $(top_srcdir)/rtems.am

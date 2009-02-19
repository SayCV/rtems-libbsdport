#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libif_em_a_SOURCES               = if_em.c
libif_em_a_SOURCES              += e1000_80003es2lan.c e1000_82540.c e1000_82541.c
libif_em_a_SOURCES              += e1000_82543.c e1000_82571.c e1000_82575.c
libif_em_a_SOURCES              += e1000_api.c e1000_mac.c e1000_manage.c
libif_em_a_SOURCES              += e1000_nvm.c e1000_phy.c

libif_em_a_SOURCES              += e1000_80003es2lan.h e1000_82541.h e1000_82543.h
libif_em_a_SOURCES              += e1000_82571.h e1000_82575.h e1000_api.h
libif_em_a_SOURCES              += e1000_defines.h e1000_hw.h e1000_ich8lan.h
libif_em_a_SOURCES              += e1000_mac.h e1000_manage.h e1000_nvm.h
libif_em_a_SOURCES              += e1000_osdep.h e1000_phy.h e1000_regs.h if_em.h

EXTRA_libif_em_a_SOURCES         = e1000_82542.c e1000_ich8lan.c

CPPFLAGS_82542_SUPPORT_NO        = -DNO_82542_SUPPORT
CPPFLAGS_ICH8LAN_SUPPORT_NO      = -DNO_ICH8LAN_SUPPORT

O_PIECES_82542_SUPPORT_YES       = e1000_82542.$(OBJEXT)
O_PIECES_ICH8LAN_SUPPORT_YES     = e1000_ich8lan.$(OBJEXT)

libif_em_a_LIBADD                =
libif_em_a_LIBADD               += $(O_PIECES_82542_SUPPORT_$(ENBL_82542_SUPPORT))
libif_em_a_LIBADD               += $(O_PIECES_ICH8LAN_SUPPORT_$(ENBL_ICH8LAN_SUPPORT))

libif_em_a_DEPENDENCIES          = $(libif_em_a_LIBADD)

lib_LIBRARIES                    = libif_em.a

AM_CPPFLAGS                     += -I$(srcdir)
AM_CPPFLAGS                     += -I$(srcdir)/../libbsdport -I../libbsdport  -I../libbsdport/dummyheaders
AM_CPPFLAGS                     += $(CPPFLAGS_82542_SUPPORT_$(ENBL_82542_SUPPORT))
AM_CPPFLAGS                     += $(CPPFLAGS_ICH8LAN_SUPPORT_$(ENBL_ICH8LAN_SUPPORT))

include $(top_srcdir)/rtems.am

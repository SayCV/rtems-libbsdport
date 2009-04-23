#  $Id$
AUTOMAKE_OPTIONS=foreign

include $(top_srcdir)/rtems-pre.am

libbsdport_a_SOURCES     = rtems_callout.c rtems_taskqueue.c rtems_udelay.c
libbsdport_a_SOURCES    += ifstuff.c devicet.c alldrv.c contigmalloc.c
libbsdport_a_SOURCES    += sysbus.c malloc.c ifmedia.c misc.c miistuff.c

libbsdport_a_SOURCES    += rtems_verscheck.h bus.h callout.h devicet.h
libbsdport_a_SOURCES    += libbsdport.h libbsdport_post.h mutex.h
libbsdport_a_SOURCES    += rtems_udelay.h rtems_verscheck.h taskqueue.h

include_bsp_HEADERS      = libbsdport_api.h

lib_LIBRARIES            = libbsdport.a

include $(top_srcdir)/rtems.am

LINKS =
LINKS+=sys/taskqueue.h
LINKS+=sys/bus.h
LINKS+=sys/mutex.h
LINKS+=bsp/rtems_verscheck.h

DUMMYHEADERS =
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

BUILT_SOURCES=

include ../links.am

#
# I found no good way to have something made first thing.
# all-local is made after 'all' and xxx_DEPENDENCIES are
# after OBJECTS are made :-(
$(libbsdport_a_OBJECTS): $(DUMMYHEADERS) $(LINKS)

if FALSE
CLOBBER_ADDITIONS  =
CLOBBER_ADDITIONS += $(srcdir)/dummyheaders
CLOBBER_ADDITIONS += $(addprefix $(srcdir)/,$(sort $(foreach n,$(LINKS),$(firstword $(subst /, ,$(n))))))

dummyheaders/%:
	@if [ ! -d $(srcdir)/`dirname $@` ] ; then mkdir -p $(srcdir)/`dirname $@`; fi
	@touch $(srcdir)/$@

# for each name listed in LINKS, create parent directories (if needed)
# and a symlink to file in .
# E.g., LINKS=a/b/c.h
# creates a/b/c.h -> ../../c.h
$(LINKS):
	@if [ ! -d $(srcdir)/$(dir $@) ] ; then mkdir -p $(srcdir)/$(dir $@); fi
	@ln -s `echo $@ | sed -e 's%[^/]\+[/]\+%../%g'` $(srcdir)/$@


distclean-local:
	$(RM) -r $(CLOBBER_ADDITIONS)
endif


AUTOMAKE_OPTIONS=foreign

ACLOCAL_AMFLAGS= -I./m4

SUBDIRS=. @enable_subdirs@

DIST_SUBDIRS= @all_subdirs@

EXTRA_DIST=makefile.top.am makefile.top.in ssrlApps.components.in

all-local:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL="$(INSTALL_IF_CHANGE)" prefix="$(abs_top_builddir)/data" exec_prefix='$$(prefix)' includedir='$$(prefix)/include' install-data
	$(MAKE) $(AM_MAKEFLAGS) INSTALL="$(INSTALL_IF_CHANGE)" prefix="$(abs_top_builddir)/data" exec_prefix='$$(prefix)' includedir='$$(prefix)/include' install-exec

clean-local:
	$(RM) -r data

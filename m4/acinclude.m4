dnl m4_syscmd is executed when aclocal is run
m4_syscmd([cat - > makefile.top.am <<'EOF_'
AUTOMAKE_OPTIONS=foreign
SUBDIRS=@the_subdirs@
# When making a distribution we only want to 
# recurse into (any) one single BSP subdir.
DIST_SUBDIRS=@the_distsub@

# The dist-hook then removes this extra
# directory level again.
dist-hook:
	if test "$(PACKAGE_VERSION)" = "untagged" ; then echo "Need tagged version to cut distribution"; exit 1; fi
	cp -frl $(distdir)/$(DIST_SUBDIRS)/* $(distdir)
	rm -fr  $(distdir)/$(DIST_SUBDIRS)
EOF_
])

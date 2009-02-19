#
# TILLAC_CVSTAG([$Name$], [pattern])
#
# Strip $Name$ from first argument extracting
# the CVS tag. If the second optional argument is
# given then it must specify a regexp pattern that
# is stripped from the resulting tag.
#
# This macro is intended to be used as follows:
#
# AC_INIT(package, TILLAC_CVSTAG([$Name$]))
#
# CVS inserts a tag which is extracted by this macro.
# Thus the CVS tag of 'configure.ac' is propagated to
# the PACKAGE_VERSION and VERSION Makefile variables.
#
# E.g., a checked-out copy may be tagged 'Release_foo'
# and using the macro:
#
# AC_INIT(package, TILLAC_CVSTAG([$Name$],'Release_'))
#
# results in the Makefile defining
#
#   PACKAGE_VERSION=foo
# 
# NOTE: if [] characters are required in the regexp pattern
# then they must be quoted ([[ ]]).
#
m4_define(TILLAC_CVSTAG,
	[m4_if(
		_TILLAC_CVSTAG($1,[$2]),
		,
		[untagged],
		_TILLAC_CVSTAG($1,[$2]))]dnl
)
m4_define(_TILLAC_CVSTAG,
	[m4_bregexp(
		[$1],
		\([[$]]Name:[[ ]]*\)\($2\)\([[^ ]]*\)\([[ ]]*[[^$]]*\)[[$]],
		\3)]dnl
)


#
# Makefile.am for ThalamicNuclei/mac_osx
#

dist_bin_SCRIPTS = \
	run_SegmentThalamicNuclei.sh

#
# We handle the  binary files differently because binary 
# files are treated as a separate compenent to the 
# repository, and thus may not be present at compile time.
#

foodir=$(prefix)/bin

install-data-local:
	cp -RL SegmentThalamicNuclei.app $(foodir)
	chmod -R 755 $(foodir)/SegmentThalamicNuclei.app

# Our release target. Include files to be excluded here. They will be
# found and removed after 'make install' is run during the 'make
# release' target.
EXCLUDE_FILES=
include $(top_srcdir)/Makefile.extra

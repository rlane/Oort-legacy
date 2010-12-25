NULL = 

#Build in these directories:

SUBDIRS = \
	sim \
	dedicated \
	ui \
	$(NULL)

riscdocdir = $(docdir)
riscdoc_DATA = \
	README \
	$(NULL)

README:
	cp -f README.md README

EXTRA_DIST = \
	$(riscdoc_DATA) \
	$(NULL)

CLEANFILES = \
	$(NULL)

DISTCLEANFILES = \
	README \
	$(NULL)

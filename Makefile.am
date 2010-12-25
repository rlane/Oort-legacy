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

benchmark: all
	RISC_SEED=0 RISC_NUM_THREADS=0 RISC_MAX_TICKS=20 valgrind --tool=callgrind --collect-atstart=no --cache-sim=yes --branch-sim=yes ./dedicated/dedicated scenarios/benchmark.lua

challenge: all
	./dedicated/dedicated scenarios/challenge01.lua solutions/challenge01.lua
	./dedicated/dedicated scenarios/challenge02.lua solutions/challenge02.lua
	./dedicated/dedicated scenarios/challenge03.lua solutions/challenge03.lua

run: all
	./dedicated/dedicated scenarios/basic.lua examples/switch.lua examples/switch.lua

run-ui: all
	./ui/ui scenarios/basic.lua examples/switch.lua examples/switch.lua

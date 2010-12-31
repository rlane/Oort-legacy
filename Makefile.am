NULL = 

AUTOMAKE_OPTIONS = foreign

#Build in these directories:

SUBDIRS = \
	sim \
	dedicated \
	ui \
	vector \
	vapi \
	test \
	$(NULL)

riscdocdir = $(docdir)
riscdoc_DATA = \
	README.md \
	$(NULL)

riscluadir = $(datadir)/risc
nobase_risclua_DATA = \
	ships.lua \
	lib.lua \
	examples/target.lua \
	examples/challenge01.lua \
	examples/switch.lua \
	examples/challenge02.lua \
	examples/demo1.lua \
	examples/challenge03.lua \
	scenarios/challenge01.lua \
	scenarios/basic.lua \
	scenarios/lib.lua \
	scenarios/furball.lua \
	scenarios/benchmark.lua \
	scenarios/challenge02.lua \
	scenarios/demo1.lua \
	scenarios/challenge03.lua \
	scenarios/missile_practice.lua \
	scenario_parser.lua \
	runtime.lua \
	solutions/challenge01.lua \
	solutions/challenge02.lua \
	solutions/challenge03.lua \
	$(NULL)

EXTRA_DIST = \
	$(nobase_risclua_DATA) \
	$(riscdoc_DATA) \
	installer.nsi \
	$(NULL)

CLEANFILES = \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

benchmark: all
	RISC_SEED=0 RISC_NUM_THREADS=0 RISC_MAX_TICKS=20 valgrind --tool=callgrind --collect-atstart=no --cache-sim=yes --branch-sim=yes ./dedicated/risc_dedicated scenarios/benchmark.lua

benchmark-threaded: all
	RISC_SEED=0 RISC_NUM_THREADS=0 RISC_MAX_TICKS=20 valgrind --tool=callgrind --collect-atstart=no --cache-sim=yes --branch-sim=yes ./dedicated/risc_dedicated scenarios/benchmark.lua

challenge: all
	./dedicated/risc_dedicated scenarios/challenge01.lua solutions/challenge01.lua
	./dedicated/risc_dedicated scenarios/challenge02.lua solutions/challenge02.lua
	./dedicated/risc_dedicated scenarios/challenge03.lua solutions/challenge03.lua

run: all
	./dedicated/risc_dedicated scenarios/basic.lua examples/switch.lua examples/switch.lua

run-ui: all
	./ui/risc_ui scenarios/basic.lua examples/switch.lua examples/switch.lua

DLL_DIR=/usr/i486-mingw32/bin

DLLS=\
	libatk-1.0-0.dll libcairo-2.dll libexpat-1.dll libfontconfig-1.dll libfreetype-6.dll \
	libgdk_pixbuf-2.0-0.dll libgdkglext-win32-1.0-0.dll libgdk-win32-2.0-0.dll libgio-2.0-0.dll \
	libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libgthread-2.0-0.dll \
	libgtkglext-win32-1.0-0.dll libgtk-win32-2.0-0.dll libiconv-2.dll libintl-8.dll libpango-1.0-0.dll \
	libpangocairo-1.0-0.dll libpangoft2-1.0-0.dll libpangowin32-1.0-0.dll libpng14.dll zlib1.dll \
	libpixman-1-0.dll glew32.dll

.PHONY: risc-win32
risc-win32: all
	rm -rf risc-win32
	mkdir risc-win32
	cp ui/risc_ui.exe dedicated/risc_dedicated.exe $(addprefix $(DLL_DIR)/,$(DLLS)) risc-win32/
	cp ships.lua runtime.lua scenario_parser.lua lib.lua risc-win32/
	dos2unix --u2d < README.md > risc-win32/README.txt
	dos2unix --u2d < TODO > risc-win32/TODO.txt
	mkdir risc-win32/scenarios
	cp scenarios/*.lua risc-win32/scenarios
	mkdir risc-win32/examples
	cp examples/*.lua risc-win32/examples

risc-installer-win32.exe: risc-win32 installer.nsi
	makensis installer.nsi

memcheck: all
	G_DEBUG=gc-friendly G_SLICE=always-malloc RISC_MAX_TICKS=10 valgrind --tool=memcheck --leak-check=full --leak-resolution=high --show-reachable=yes --num-callers=20 --log-file=memcheck.out ./dedicated/risc_dedicated scenarios/basic.lua examples/switch.lua examples/switch.lua
	@echo "Memcheck output saved to memcheck.out"

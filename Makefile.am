NULL = 

AUTOMAKE_OPTIONS = foreign

#Build in these directories:

SUBDIRS = \
	luajit_build \
	cjson \
	vector \
	sim \
	dedicated \
	ui \
	vapi \
	test \
	$(NULL)

oortdocdir = $(docdir)
oortdoc_DATA = \
	README.md \
	$(NULL)

oortluadir = $(datadir)/oort
nobase_oortlua_DATA = \
	ships.lua \
	lib.lua \
	examples/target.lua \
	examples/reference.lua \
	scenarios/challenge01.json \
	scenarios/basic.json \
	scenarios/furball2.json \
	scenarios/furball3.json \
	scenarios/furball4.json \
	scenarios/benchmark.json \
	scenarios/challenge02.json \
	scenarios/demo1.json \
	scenarios/challenge03.json \
	runtime.lua \
	solutions/challenge01.lua \
	solutions/challenge02.lua \
	solutions/challenge03.lua \
	$(NULL)

EXTRA_DIST = \
	$(nobase_oortlua_DATA) \
	$(oortdoc_DATA) \
	installer.nsi \
	$(NULL)

CLEANFILES = \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

benchmark: all
	OORT_NUM_THREADS=0 valgrind --tool=callgrind --collect-atstart=no ./dedicated/oort_dedicated -s 1 --max-ticks 20 scenarios/benchmark.json

benchmark-cache: all
	OORT_NUM_THREADS=0 valgrind --tool=callgrind --collect-atstart=no --cache-sim=yes --branch-sim=yes ./dedicated/oort_dedicated -s 1 --max-ticks 20 scenarios/benchmark.json

challenge: all
	./dedicated/oort_dedicated scenarios/challenge01.json solutions/challenge01.lua
	./dedicated/oort_dedicated scenarios/challenge02.json solutions/challenge02.lua
	./dedicated/oort_dedicated scenarios/challenge03.json solutions/challenge03.lua
	./dedicated/oort_dedicated scenarios/challenge04.json solutions/challenge04.lua

run: all
	./dedicated/oort_dedicated scenarios/basic.json examples/reference.lua examples/reference.lua

run-ui: all
	./ui/oort scenarios/basic.json examples/reference.lua examples/reference.lua

DLL_DIR=/usr/i486-mingw32/bin

DLLS=\
	libatk-1.0-0.dll libcairo-2.dll libexpat-1.dll libfontconfig-1.dll libfreetype-6.dll \
	libgdk_pixbuf-2.0-0.dll libgdkglext-win32-1.0-0.dll libgdk-win32-2.0-0.dll libgio-2.0-0.dll \
	libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libgthread-2.0-0.dll \
	libgtkglext-win32-1.0-0.dll libgtk-win32-2.0-0.dll libiconv-2.dll libintl-8.dll libpango-1.0-0.dll \
	libpangocairo-1.0-0.dll libpangoft2-1.0-0.dll libpangowin32-1.0-0.dll libpng14.dll zlib1.dll \
	libpixman-1-0.dll glew32.dll

.PHONY: oort-win32
oort-win32: all
	rm -rf oort-win32
	mkdir oort-win32
	cp ui/oort.exe dedicated/oort_dedicated.exe $(addprefix $(DLL_DIR)/,$(DLLS)) oort-win32/
	cp ships.lua runtime.lua lib.lua oort-win32/
	dos2unix --u2d < README.md > oort-win32/README.txt
	mkdir oort-win32/scenarios
	cp scenarios/*.json oort-win32/scenarios
	mkdir oort-win32/examples
	cp examples/*.lua oort-win32/examples

oort-installer-win32.exe: oort-win32 installer.nsi
	makensis installer.nsi

memcheck: all
	G_DEBUG=gc-friendly G_SLICE=always-malloc valgrind --tool=memcheck --leak-check=full --leak-resolution=high --show-reachable=yes --num-callers=20 --log-file=memcheck.out ./dedicated/oort_dedicated --max-ticks 10 scenarios/basic.json examples/reference.lua examples/reference.lua
	@echo "Memcheck output saved to memcheck.out"

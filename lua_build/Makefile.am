EXTRA_DIST = manifest-dist $(shell cat manifest-dist)

all-am: lua-stamp

manifest-dist:
	find ../lua -path ../lua/.git -prune -o -type f -a -print > manifest-dist

lua-stamp:
if MINGW32
	cd $(top_srcdir)/lua/src && make PLAT=mingw MYCFLAGS="-g" CC=$(CC) clean liblua.a
else
	cd $(top_srcdir)/lua/src && make PLAT=generic MYCFLAGS="-g" CC=$(CC) clean liblua.a
endif
	touch lua-stamp

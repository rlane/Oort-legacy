EXTRA_DIST = manifest-dist $(shell cat manifest-dist)
lua_dir = ../lua

all-am: lua-stamp

manifest-dist:
	find $(lua_dir) -path $(lua_dir)/.git -prune -o -type f -a -print > manifest-dist

lua-stamp:
if MINGW32
	make -C $(lua_dir)/src PLAT=mingw MYCFLAGS="$(CFLAGS)" CC=$(CC) clean liblua.a
else
	make -C $(lua_dir)/src PLAT=generic MYCFLAGS="$(CFLAGS)" CC=$(CC) clean liblua.a
endif
	touch lua-stamp

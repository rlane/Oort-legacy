EXTRA_DIST = manifest-dist $(shell cat manifest-dist)

all-am: lua-stamp

manifest-dist:
	find ../lua -path ../lua/.git -prune -o -type f -a -print > manifest-dist

lua-stamp:
if MINGW32
	cd $(top_srcdir)/lua && make PLAT=mingw MYCFLAGS="-g -DLUA_USE_APICHECK" clean all
else
	cd $(top_srcdir)/lua && make PLAT=linux MYCFLAGS="-g -DLUA_USE_APICHECK" clean all
endif
	touch lua-stamp

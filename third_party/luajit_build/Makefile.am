EXTRA_DIST = manifest-dist $(shell cat manifest-dist)
luajit_dir = $(top_srcdir)/third_party/luajit

all-am: luajit-stamp

manifest-dist:
	find ../luajit -path ../luajit/.git -prune -o -type f -a -print > manifest-dist

luajit-stamp:
if MINGW32
	make -C $(luajit_dir) BUILDMODE=static Q= HOST_CC="gcc -m32" CROSS=`echo $(CC) | sed -e 's/gcc//'` TARGET_SYS="Windows" all
else
	make -C $(luajit_dir) BUILDMODE=static CC=$(CC) Q= CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" all
endif
	touch luajit-stamp

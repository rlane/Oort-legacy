EXTRA_DIST = manifest-dist $(shell cat manifest-dist)

all-am: luajit-stamp

manifest-dist:
	find ../luajit -path ../luajit/.git -prune -o -type f -a -print > manifest-dist

luajit-stamp:
	cd $(top_srcdir)/luajit && make amalg
	touch luajit-stamp

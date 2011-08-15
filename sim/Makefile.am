NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(VALGRIND_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/cjson \
	-I $(top_srcdir)/lua/src \
	$(NULL)

AM_VALAFLAGS = \
	--library oort -H oort.h \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/cjson \
	--vapidir $(top_srcdir)/sim \
	--pkg lua \
	--pkg util \
	--pkg vector \
	--pkg gio-2.0 \
	--pkg cjson \
	--pkg config \
	--thread \
	$(NULL)

lib_LIBRARIES = libsim.a

libsim_a_SOURCES = \
	bullet.vala \
	beam.vala \
	game.vala \
	physics.vala \
	scenario.vala \
	ship_class.vala \
	ship.vala \
	task.vala \
	team.vala \
	util.c \
	paths.vala \
	$(NULL)

noinst_HEADERS = util.h oort.h oort.vapi util.vapi

libsim_a_LDFLAGS = $(OORT_LIBS)
libsim_a_LIBADD = $(top_srcdir)/cjson/libcjson.a $(top_srcdir)/lua/src/liblua.a

oort.h: libsim_a_vala.stamp

EXTRA_DIST = \
	$(NULL)

CLEANFILES = \
	*.o
	$(NULL)

DISTCLEANFILES = \
	bullet.c \
	beam.c \
	game.c \
	physics.c \
	scenario.c \
	ship_class.c \
	ship.c \
	task.c \
	team.c \
	sim.h \
	$(NULL)

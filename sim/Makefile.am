NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	$(RISC_LUA_CFLAGS) \
	$(VALGRIND_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/cjson \
	$(NULL)

AM_VALAFLAGS = \
	--library risc -H risc.h \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/cjson \
	--vapidir $(top_srcdir)/sim \
	--pkg lua \
	--pkg util \
	--pkg vector \
	--pkg gio-2.0 \
	--pkg cjson \
	--thread \
	$(NULL)

lib_LIBRARIES = libsim.a

libsim_a_SOURCES = \
	bullet.vala \
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

noinst_HEADERS = util.h risc.h risc.vapi util.vapi

libsim_a_LDFLAGS = $(RISC_LIBS) $(RISC_LUA_LIBS)
libsim_a_LIBADD = $(top_srcdir)/cjson/libcjson.a

risc.h: libsim_a_vala.stamp

EXTRA_DIST = \
	$(NULL)

CLEANFILES = \
	*.o
	$(NULL)

DISTCLEANFILES = \
	bullet.c \
	game.c \
	physics.c \
	scenario.c \
	ship_class.c \
	ship.c \
	task.c \
	team.c \
	sim.h \
	$(NULL)

NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	$(RISC_LUA_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	$(NULL)

AM_VALAFLAGS = \
	--library risc -H risc.h \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	--pkg lua \
	--pkg csim \
	--pkg vector \
	--pkg gio-2.0 \
	--thread \
	$(NULL)

lib_LTLIBRARIES = librisc-1.0.la

librisc_1_0_la_SOURCES = \
	bullet.vala \
	game.vala \
	physics.vala \
	scenario.vala \
	ship_class.vala \
	ship_vala.vala \
	task.vala \
	team.vala \
	ship.c \
	util.c \
	paths.vala \
	$(NULL)

noinst_HEADERS = ship.h util.h risc.h risc.vapi csim.vapi

librisc_1_0_la_LDFLAGS = $(RISC_LIBS) $(RISC_LUA_LIBS)

risc.h: librisc_1_0_la_vala.stamp

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
	ship_vala.c \
	task.c \
	team.c \
	sim.h \
	$(NULL)

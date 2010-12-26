NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
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
	api_team.c \
	api_sensors.c \
	util.c \
	$(NULL)

librisc_1_0_la_LDFLAGS = `pkg-config --libs glib-2.0 gthread-2.0 gobject-2.0 lua`

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

munge_vapi: librisc_1_0_la_vala.stamp
	mv risc.vapi risc.vapi.tmp
	echo '[CCode (cheader_filename = "risc.h")]' > risc.vapi
	cat risc.vapi.tmp >> risc.vapi
	rm risc.vapi.tmp

all-am: munge_vapi

NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	$(NULL)

AM_VALAFLAGS = \
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

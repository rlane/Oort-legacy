NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(LUA_CFLAGS) \
	$(BOX2D_CFLAGS) \
	$(NULL)

noinst_LIBRARIES = libsim.a

libsim_a_SOURCES = \
	game.cc \
	entity.cc \
	ship.cc \
	scenario.cc \
	ai.cc \
	$(NULL)

noinst_HEADERS = game.h ship.h team.h ai.h scenario.h entity.h

libsim_a_LDFLAGS =
libsim_a_LIBADD =

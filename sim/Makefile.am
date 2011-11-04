NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(NULL)

noinst_LIBRARIES = libsim.a

libsim_a_SOURCES = \
	game.cc \
	physics.cc \
	ship.cc \
	scenario.cc \
	$(NULL)

noinst_HEADERS = game.h physics.h ship.h team.h ai.h scenario.h

libsim_a_LDFLAGS =
libsim_a_LIBADD =

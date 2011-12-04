NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(GLEW_CFLAGS)
	$(BOX2D_CFLAGS) \
	$(NULL)

noinst_LIBRARIES = librenderer.a

librenderer_a_SOURCES = \
	renderer.cc \
	physics_debug_renderer.cc \
	$(NULL)

noinst_HEADERS = renderer.h physics_debug_renderer.h

librenderer_a_LDFLAGS =
librenderer_a_LIBADD =

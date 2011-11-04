NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(GLEW_CFLAGS)
	$(NULL)

noinst_LIBRARIES = librenderer.a

librenderer_a_SOURCES = \
	renderer.cc \
	$(NULL)

noinst_HEADERS = renderer.h

librenderer_a_LDFLAGS =
librenderer_a_LIBADD =

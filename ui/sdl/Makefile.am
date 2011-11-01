NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/sim \
	$(GLM_CFLAGS) \
	$(SDL_CFLAGS)
	$(GLEW_CFLAGS)
	$(NULL)

LIBS=$(SDL_LIBS) $(GLEW_LIBS)

bin_PROGRAMS = oort_sdl
oort_sdl_SOURCES = main.cc
oort_sdl_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/renderer/librenderer.a

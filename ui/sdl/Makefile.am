NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(SDL_CFLAGS)
	$(GLEW_CFLAGS)
	$(NULL)

LIBS=$(SDL_LIBS) $(GLEW_LIBS) $(LUA_LIBS) $(BOX2D_LIBS)
INTERNAL_LIBS=$(top_builddir)/sim/libsim.a $(top_builddir)/renderer/librenderer.a

bin_PROGRAMS = oort_sdl
oort_sdl_SOURCES = main.cc
oort_sdl_LDADD =
oort_sdl_LDFLAGS = -Wl,--whole-archive $(INTERNAL_LIBS) -Wl,--no-whole-archive

$(bin_PROGRAMS): $(INTERNAL_LIBS)

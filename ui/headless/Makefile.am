NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(NULL)

LIBS=$(LUA_LIBS) $(BOX2D_LIBS)

bin_PROGRAMS = oort_headless
oort_headless_SOURCES = main.cc
oort_headless_LDADD =
oort_headless_LDFLAGS = -Wl,--whole-archive $(top_builddir)/sim/libsim.a -Wl,--no-whole-archive

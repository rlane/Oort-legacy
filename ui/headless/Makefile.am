NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/sim \
	$(GLM_CFLAGS) \
	$(NULL)

bin_PROGRAMS = oort_headless
oort_headless_SOURCES = main.cc
oort_headless_LDADD = $(top_srcdir)/sim/libsim.a

noinst_HEADERS = math3d.vapi math3d.h

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(VALGRIND_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/math3d \
	$(NULL)

noinst_LIBRARIES = libmath3d.a

libmath3d_a_SOURCES = \
	math3d.c \
	$(NULL)

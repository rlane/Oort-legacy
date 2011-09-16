noinst_HEADERS = vector.vapi vec2d.h vec4f.h

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(VALGRIND_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	$(NULL)

AM_VALAFLAGS = -H vector_gen.h

lib_LIBRARIES = libvector.a

libvector_a_SOURCES = \
	mat4f.vala \
	$(NULL)

vector_gen.h: libvector_a_vala.stamp

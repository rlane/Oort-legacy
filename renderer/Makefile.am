NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(VALGRIND_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-include oort.h \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/math3d \
	-I $(top_srcdir)/sim \
	$(CJSON_CFLAGS) \
	$(LUA_CFLAGS) \
	$(NULL)

AM_VALAFLAGS = \
	--library oort_renderer -H oort_renderer.h \
	--pkg oort \
	--pkg lua \
	--pkg vector \
	--pkg util \
	--pkg gtk+-2.0 \
	--pkg gtkglext-1.0 \
	--pkg glew \
	--pkg gl \
	--pkg particle \
	--pkg glutil \
	--pkg gio-2.0 \
	--pkg font \
	--pkg config \
	--pkg cjson \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	--vapidir $(top_srcdir)/ui \
	--vapidir $(top_srcdir)/renderer \
	--vapidir $(CJSON_DIR) \
	$(NULL)

noinst_LIBRARIES = librenderer.a

librenderer_a_SOURCES = \
	renderer.vala \
	glutil.c \
	particle.c \
	font.c \
	textures.vala \
	model.vala \
	shader.vala \
	perf.vala \
	batch.vala \
	batches/ship.vala \
	batches/particle_batch.vala \
	batches/tail.vala \
	batches/boundary.vala \
	batches/bullet.vala \
	batches/beam.vala

noinst_HEADERS = \
	glutil.h \
	particle.h \
	tga.h \
	font.h \
	font.vapi \
	particle.vapi \
	glutil.vapi

#libsim_a_LDFLAGS = $(OORT_LIBS)
#libsim_a_LIBADD = $(CJSON_LDADD) $(LUA_LDADD)

oort_renderer.h: librenderer_a_vala.stamp

EXTRA_DIST = \
	$(NULL)

CLEANFILES = \
	*.o
	$(NULL)

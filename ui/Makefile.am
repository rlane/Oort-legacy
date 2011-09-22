NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(OORT_UI_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-include oort.h \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/math3d \
	-I $(top_srcdir)/cjson \
	-I $(top_srcdir)/sim \
	-I $(top_srcdir)/luajit/src \
	$(NULL)

LIBS = $(OORT_LIBS) $(OORT_UI_LIBS) -lm

AM_VALAFLAGS = \
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
	--vapidir $(top_srcdir)/cjson \
	$(NULL)

bin_PROGRAMS = oort
oort_SOURCES = oort.vala renderer.vala glutil.c particle.c font.c textures.vala log.vala model.vala shader.vala perf.vala batch.vala batches/ship.vala
noinst_HEADERS = glutil.h particle.h tga.h font.h font.vapi particle.vapi glutil.vapi
oort_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/cjson/libcjson.a $(top_srcdir)/luajit/src/libluajit.a $(top_srcdir)/math3d/libmath3d.a
oort_LDFLAGS = $(OORT_EXTRA_LDFLAGS)

NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(OORT_UI_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-include oort.h \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	-I $(top_srcdir)/lua/src \
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
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	--vapidir $(top_srcdir)/ui \
	$(NULL)

bin_PROGRAMS = oort
oort_SOURCES = oort.vala renderer.vala glutil.c particle.c font.c textures.vala log.vala
noinst_HEADERS = glutil.h particle.h tga.h font.h font.vapi particle.vapi glutil.vapi
oort_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/cjson/libcjson.a $(top_srcdir)/lua/src/liblua.a
oort_LDFLAGS = $(OORT_EXTRA_LDFLAGS)

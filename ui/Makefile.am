NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	$(OORT_UI_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-include oort.h \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/math3d \
	-I $(top_srcdir)/sim \
	-I $(top_srcdir)/renderer \
	$(CJSON_CFLAGS) \
	$(LUA_CFLAGS) \
	$(NULL)

LIBS = $(OORT_LIBS) $(OORT_UI_LIBS) -lm

AM_VALAFLAGS = \
	--pkg oort \
	--pkg oort_renderer \
	--pkg lua \
	--pkg vector \
	--pkg util \
	--pkg gtk+-2.0 \
	--pkg gtkglext-1.0 \
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
	--vapidir $(top_srcdir)/renderer \
	--vapidir $(top_srcdir)/ui \
	--vapidir $(CJSON_DIR) \
	$(NULL)

bin_PROGRAMS = oort
oort_SOURCES = oort.vala log.vala
oort_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/renderer/librenderer.a $(CJSON_LDADD) $(LUA_LDADD) $(top_srcdir)/math3d/libmath3d.a
oort_LDFLAGS = $(OORT_EXTRA_LDFLAGS)

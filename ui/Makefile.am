NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	$(RISC_UI_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-include risc.h \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	$(NULL)

LIBS = $(RISC_LIBS) $(RISC_UI_LIBS) $(RISC_LUA_LIBS)

AM_VALAFLAGS = \
	--pkg risc \
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
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	--vapidir $(top_srcdir)/ui \
	$(NULL)

bin_PROGRAMS = risc_ui
risc_ui_SOURCES = risc.vala renderer.vala glutil.c particle.c font.c
noinst_HEADERS = glutil.h particle.h tga.h font.h font.vapi particle.vapi glutil.vapi
risc_ui_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/cjson/libcjson.a
risc_ui_LDFLAGS = $(RISC_EXTRA_LDFLAGS) -static

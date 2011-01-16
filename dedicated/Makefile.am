NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	-include risc.h \
	$(NULL)

AM_VALAFLAGS = \
	--pkg risc \
	--pkg lua \
	--pkg vector \
	--pkg util \
	--pkg gio-2.0 \
	--pkg config \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	$(NULL)

LIBS=$(RISC_LIBS) -lm

bin_PROGRAMS = risc_dedicated
risc_dedicated_SOURCES = risc-dedicated.vala
risc_dedicated_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/cjson/libcjson.a $(top_srcdir)/luajit/src/libluajit.a
risc_dedicated_LDFLAGS = $(RISC_EXTRA_LDFLAGS)

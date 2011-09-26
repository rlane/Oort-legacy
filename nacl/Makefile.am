NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	-I $(top_srcdir)/lua/src \
	-include oort.h \
	$(NULL)

AM_VALAFLAGS = \
	--pkg oort \
	--pkg lua \
	--pkg vector \
	--pkg util \
	--pkg gio-2.0 \
	--pkg config \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	$(NULL)

LIBS=$(OORT_LIBS) -lm -lppapi

bin_PROGRAMS = oort_nacl
oort_nacl_SOURCES = main.c
oort_nacl_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/cjson/libcjson.a $(top_srcdir)/lua/src/liblua.a
oort_nacl_LDFLAGS = $(OORT_EXTRA_LDFLAGS)

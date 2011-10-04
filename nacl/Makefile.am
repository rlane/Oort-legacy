NULL = 

AM_CFLAGS = \
	$(OORT_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	$(LUA_CFLAGS) \
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

LIBS=$(OORT_LIBS) -lm -lppapi -lnosys

bin_PROGRAMS = oort_nacl
oort_nacl_SOURCES = nacl.c main.vala embedded_resources.c
oort_nacl_LDADD = $(top_srcdir)/sim/libsim.a $(CJSON_LDADD) $(LUA_LDADD)
oort_nacl_LDFLAGS = $(OORT_EXTRA_LDFLAGS)

embedded_resources.c: $(top_srcdir)/resources-manifest
	cd .. && $(RUBY) third_party/insrc/insrc.rb resources-manifest > nacl/embedded_resources.c

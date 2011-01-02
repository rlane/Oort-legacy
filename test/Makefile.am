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
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	$(NULL)

LIBS=$(RISC_LIBS) $(RISC_LUA_LIBS)

TESTS = test_syntax_errors
check_PROGRAMS = test_syntax_errors
test_syntax_errors_SOURCES = test_syntax_errors.vala
test_syntax_errors_LDADD = $(top_srcdir)/sim/libsim.a $(top_srcdir)/cjson/libcjson.a
test_syntax_errors_LDFLAGS = $(RISC_EXTRA_LDFLAGS)

EXTRA_DIST = \
	ai/syntax_error.lua \
	scenarios/syntax_error.json \
	scenarios/simple.json \
	$(NULL)

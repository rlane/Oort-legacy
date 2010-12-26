NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	$(NULL)

AM_VALAFLAGS = \
	--pkg risc \
	--pkg lua \
	--pkg vector \
	--pkg csim \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	$(NULL)

bin_PROGRAMS = risc_dedicated
risc_dedicated_SOURCES = risc-dedicated.vala
risc_dedicated_LDADD = $(top_srcdir)/sim/librisc-1.0.la

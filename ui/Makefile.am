NULL = 

AM_CFLAGS = \
	$(RISC_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir)/vector \
	-I $(top_srcdir)/sim \
	`pkg-config --cflags gtk+-2.0 gtkglext-1.0 glew` \
	$(NULL)

AM_LDFLAGS = \
	`pkg-config --libs gtk+-2.0 gtkglext-1.0 glew` \
	$(NULL)

AM_VALAFLAGS = \
	--pkg risc \
	--pkg lua \
	--pkg vector \
	--pkg csim \
	--pkg gtk+-2.0 \
	--pkg gtkglext-1.0 \
	--pkg glew \
	--pkg gl \
	--pkg particle \
	--pkg glutil \
	--vapidir $(top_srcdir)/vapi \
	--vapidir $(top_srcdir)/vector \
	--vapidir $(top_srcdir)/sim \
	--vapidir $(top_srcdir)/ui \
	$(NULL)

bin_PROGRAMS = ui
ui_SOURCES = risc.vala renderer.vala glutil.c particle.c
ui_LDADD = $(top_srcdir)/sim/librisc-1.0.la

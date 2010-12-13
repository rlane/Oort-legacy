# Ubuntu 10.10
# core: build-essential pkg-config libglib2.0-dev libluajit-5.1-dev
# ui: valac libglew1.5-dev libgtk2.0-dev libgtkglext1-dev libsdl-gfx1.2-dev

LUAPKG:=$(shell pkg-config --exists luajit && echo luajit || echo lua)
VALGRIND_CFLAGS=$(shell pkg-config --exists valgrind && echo -D VALGRIND `pkg-config --cflags valgrind`)
CORE_PKGS:=glib-2.0 gthread-2.0 gobject-2.0 $(LUAPKG)
UI_PKGS:=gtk+-2.0 gtkglext-1.0 glew # SDL_gfx

CORE_CFLAGS:=`pkg-config --cflags $(CORE_PKGS)` -g -O2 -march=native -Wall -I. $(VALGRIND_CFLAGS)
CORE_LDFLAGS:=`pkg-config --libs $(CORE_PKGS)`

UI_CFLAGS:=$(CORE_CFLAGS) `pkg-config --cflags $(UI_PKGS)`
UI_LDFLAGS:=$(CORE_LDFLAGS) `pkg-config --libs $(UI_PKGS)` -lGL -lGLU

core_sources = physics.c ship.c task.c team.c util.c api_sensors.c api_team.c
core_vala = game.vala bullet.vala scenario.vala
core_objects = $(core_sources:.c=.o) $(core_vala:.vala=.o)

dedicated_sources =
dedicated_vala = risc-dedicated.vala
dedicated_objects = $(dedicated_sources:.c=.o) $(dedicated_vala:.vala=.o)

ui_sources = particle.c gl13.c glutil.c
ui_vala = risc.vala renderer.vala
ui_objects = $(ui_sources:.c=.o) $(ui_vala:.vala=.o)

vapi = vapi/vector.vapi

all: luacheck risc risc-dedicated

luacheck:
	@echo using Lua package $(LUAPKG)

%.d: %.c
				@set -e; rm -f $@; \
				$(CC) -M $(CORE_CFLAGS) $< > $@.$$$$; \
				sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
				rm -f $@.$$$$

-include $(core_sources:.c=.d)

$(core_vala:.vala=.c) risc.h vapi/risc.vapi: $(core_vala) $(vapi) vapi/cisc.vapi
	valac --library risc --basedir ./ -H risc.h --vapi vapi/risc.vapi -C --pkg lua --pkg cisc --pkg vector --vapidir vapi $(core_vala)
	mv vapi/risc.vapi vapi/risc.vapi.tmp
	echo '[CCode (cheader_filename = "risc.h")]' > vapi/risc.vapi
	cat vapi/risc.vapi.tmp >> vapi/risc.vapi
	rm vapi/risc.vapi.tmp

$(ui_vala:.vala=.c): $(ui_vala) $(vapi) vapi/cisc.vapi vapi/risc.vapi
	valac -C --pkg gtk+-2.0 --pkg gtkglext-1.0 --pkg lua --pkg risc --pkg cisc --pkg gl --pkg vector --vapidir vapi $(ui_vala)

$(dedicated_vala:.vala=.c): $(dedicated_vala) $(vapi) vapi/cisc.vapi vapi/risc.vapi
	valac -C --pkg lua --pkg risc --pkg vector --pkg cisc --vapidir vapi $(dedicated_vala)

$(core_objects) : CFLAGS = $(CORE_CFLAGS)
$(dedicated_objects) : CFLAGS = $(CORE_CFLAGS)
$(ui_objects) : CFLAGS = $(UI_CFLAGS)

risc: LDFLAGS = $(UI_LDFLAGS)
risc: $(ui_objects) $(core_objects)

risc-dedicated: LDFLAGS = $(CORE_LDFLAGS)
risc-dedicated: $(dedicated_objects) $(core_objects)

benchmark: risc-dedicated
	RISC_SEED=0 RISC_NUM_THREADS=0 RISC_MAX_TICKS=20 valgrind --tool=callgrind --collect-atstart=no --cache-sim=yes --branch-sim=yes ./risc-dedicated scenarios/benchmark.lua

challenge: risc-dedicated
	./risc-dedicated scenarios/challenge01.lua solutions/challenge01.lua
	./risc-dedicated scenarios/challenge02.lua solutions/challenge02.lua
	./risc-dedicated scenarios/challenge03.lua solutions/challenge03.lua

run: risc-dedicated
	./risc-dedicated scenarios/basic.lua examples/switch.lua examples/switch.lua

run-ui: risc
	./risc scenarios/basic.lua examples/switch.lua examples/switch.lua

install: risc risc-dedicated
	install -d $(DESTDIR)/usr/bin
	install risc risc-dedicated $(DESTDIR)/usr/bin
	install -d $(DESTDIR)/usr/share/risc
	install ships.lua runtime.lua scenario_parser.lua $(DESTDIR)/usr/share/risc
	install -d $(DESTDIR)/usr/share/risc/examples
	install examples/*.lua $(DESTDIR)/usr/share/risc/examples
	install -d $(DESTDIR)/usr/share/risc/scenarios
	install scenarios/*.lua $(DESTDIR)/usr/share/risc/scenarios

clean:
	rm -f risc.h vapi/risc.vapi
	rm -f $(core_vala:.vala=.c) $(ui_vala:.vala=.c) $(dedicated_vala:.vala=.c)
	rm -f *.o *.d risc risc-dedicated

test_check_collision: CFLAGS = $(CORE_CFLAGS)
test_check_collision: LDFLAGS = $(CORE_LDFLAGS)
test_check_collision: test_check_collision.o physics.o
	gcc -o test_check_collision test_check_collision.o physics.o $(LDFLAGS)

.PHONY: all clean benchmark luacheck install run run-ui

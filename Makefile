# Ubuntu 10.10
# core: build-essential pkg-config libglib2.0-dev libluajit-5.1-dev
# ui: valac libglew1.5-dev libgtk2.0-dev libgtkglext1-dev libsdl-gfx1.2-dev

LUAPKG:=$(shell pkg-config --exists luajit && echo luajit || echo lua)
VALGRIND_CFLAGS=$(shell pkg-config --exists valgrind && echo -D VALGRIND `pkg-config --cflags valgrind`)
CORE_PKGS:=glib-2.0 gthread-2.0 $(LUAPKG)
UI_PKGS:=gtk+-2.0 gtkglext-1.0 glew # SDL_gfx

CORE_CFLAGS:=`pkg-config --cflags $(CORE_PKGS)` -g -O2 -march=native -Wall -I. $(VALGRIND_CFLAGS)
CORE_LDFLAGS:=`pkg-config --libs $(CORE_PKGS)`

UI_CFLAGS:=$(CORE_CFLAGS) `pkg-config --cflags $(UI_PKGS)`
UI_LDFLAGS:=$(CORE_LDFLAGS) `pkg-config --libs $(UI_PKGS)` -lGL -lGLU

core_sources = bullet.c  game.c  physics.c  scenario.c  ship.c  task.c team.c util.c api_sensors.c api_team.c
core_objects = $(core_sources:.c=.o)

ui_sources = particle.c gl13.c glutil.c
ui_vala = risc.vala renderer.vala
ui_objects = $(ui_sources:.c=.o) $(ui_vala:.vala=.o)

all: luacheck risc risc-dedicated

luacheck:
	@echo using Lua package $(LUAPKG)

%.d: %.c
				@set -e; rm -f $@; \
				$(CC) -M $(CORE_CFLAGS) $< > $@.$$$$; \
				sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
				rm -f $@.$$$$

-include $(core_sources:.c=.d)

$(ui_vala:.vala=.c): $(ui_vala) vapi/risc.vapi vapi/vector.vapi
	valac -C --pkg gtk+-2.0 --pkg gtkglext-1.0 --pkg lua --pkg risc --pkg gl --pkg vector --vapidir vapi $(ui_vala)

$(core_objects) risc-dedicated.o : CFLAGS = $(CORE_CFLAGS)
$(ui_objects) risc.o renderer.o : CFLAGS = $(UI_CFLAGS)

risc: LDFLAGS = $(UI_LDFLAGS)
risc: risc.o renderer.o $(ui_objects) $(core_objects)

risc-dedicated: LDFLAGS = $(CORE_LDFLAGS)
risc-dedicated: risc-dedicated.o $(core_objects)

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
	rm -f *.o *.d risc risc-dedicated risc.c renderer.c

.PHONY: all clean benchmark luacheck install run run-ui

# Ubuntu 10.10
# core: build-essential pkg-config libglib2.0-dev libluajit-5.1-dev
# ui: valac libglew1.5-dev libgtk2.0-dev libgtkglext1-dev libsdl-gfx1.2-dev

LUAPKG:=$(shell pkg-config --exists luajit && echo luajit || echo lua)
VALGRIND_CFLAGS=$(shell pkg-config --exists valgrind && echo -D VALGRIND `pkg-config --cflags valgrind`)
CORE_PKGS:=glib-2.0 gthread-2.0 gobject-2.0 $(LUAPKG)
UI_PKGS:=gtk+-2.0 gtkglext-1.0 glew # SDL_gfx

CORE_CFLAGS:=`pkg-config --cflags $(CORE_PKGS)` -g -O2 -march=native -Wall -I. -Ivector -Isim $(VALGRIND_CFLAGS)
CORE_LDFLAGS:=`pkg-config --libs $(CORE_PKGS)`

UI_CFLAGS:=$(CORE_CFLAGS) `pkg-config --cflags $(UI_PKGS)` -Iui
UI_LDFLAGS:=$(CORE_LDFLAGS) `pkg-config --libs $(UI_PKGS)` -lGL -lGLU

core_sources = sim/ship.c sim/util.c sim/api_sensors.c sim/api_team.c
core_vala = sim/game.vala sim/bullet.vala sim/scenario.vala sim/team.vala sim/physics.vala sim/task.vala sim/ship_class.vala sim/ship_vala.vala
core_objects = $(core_sources:.c=.o) $(core_vala:.vala=.o)

dedicated_sources =
dedicated_vala = dedicated/risc-dedicated.vala
dedicated_objects = $(dedicated_sources:.c=.o) $(dedicated_vala:.vala=.o)

ui_sources = ui/particle.c ui/glutil.c
ui_vala = ui/risc.vala ui/renderer.vala
ui_objects = $(ui_sources:.c=.o) $(ui_vala:.vala=.o)

vapi = vector/vector.vapi sim/csim.vapi

all: luacheck risc risc-dedicated

luacheck:
	@echo using Lua package $(LUAPKG)

%.d: %.c
				@set -e; rm -f $@; \
				$(CC) -M $(CORE_CFLAGS) $< > $@.$$$$; \
				sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
				rm -f $@.$$$$

#-include $(core_sources:.c=.d)

.stamp.core: $(core_vala) 
	valac --library risc --basedir ./ -H risc.h --vapi sim/sim.vapi -C --pkg lua --pkg csim --pkg vector --vapidir vapi --vapidir vector --vapidir sim $(core_vala)
	mv sim/sim.vapi sim/sim.vapi.tmp
	echo '[CCode (cheader_filename = "risc.h")]' > sim/sim.vapi
	cat sim/sim.vapi.tmp >> sim/sim.vapi
	rm sim/sim.vapi.tmp

$(core_vala:.vala=.c) risc.h sim/sim.vapi: .stamp.core

.stamp.ui: $(ui_vala) $(vapi) sim/sim.vapi
	valac -C --pkg gtk+-2.0 --pkg gtkglext-1.0 --pkg lua --pkg sim --pkg csim --pkg glew --pkg gl --pkg vector --pkg particle --pkg glutil --vapidir vapi --vapidir vector --vapidir sim --vapidir ui $(ui_vala)

$(ui_vala:.vala=.c): .stamp.ui

.stamp.dedicated: $(dedicated_vala) $(vapi) sim/sim.vapi
	valac -C --pkg lua --pkg sim --pkg vector --pkg csim --vapidir vapi --vapidir vector --vapidir sim $(dedicated_vala)

$(dedicated_vala:.vala=.c): .stamp.dedicated

$(core_objects) : CFLAGS = $(CORE_CFLAGS)
$(dedicated_objects) : CFLAGS = $(CORE_CFLAGS)
$(ui_objects) : CFLAGS = $(UI_CFLAGS)

risc: $(ui_objects) $(core_objects)
	$(CC) $(UI_LDFLAGS) $^ -o $@

risc-dedicated: $(dedicated_objects) $(core_objects)
	$(CC) $(CORE_LDFLAGS) $^ -o $@

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
	rm -f risc.h sim/sim.vapi
	rm -f $(core_vala:.vala=.c) $(ui_vala:.vala=.c) $(dedicated_vala:.vala=.c)
	rm -f $(core_objects) $(ui_objects) $(dedicated_objects)
	rm -f */*.d.*
	rm -f .stamp.*

test_check_collision: CFLAGS = $(CORE_CFLAGS)
test_check_collision: LDFLAGS = $(CORE_LDFLAGS)
test_check_collision: test_check_collision.o physics.o
	gcc -o test_check_collision test_check_collision.o physics.o $(LDFLAGS)

.PHONY: all clean benchmark luacheck install run run-ui

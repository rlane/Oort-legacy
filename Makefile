LUA=lua5.1

CFLAGS=-I/usr/include/lua5.1 \
       `sdl-config --cflags` \
       `pkg-config glib-2.0 --cflags` \
       -g -O2 -march=native -Wall

LDFLAGS=-l$(LUA) \
        `sdl-config --libs` \
        `pkg-config glib-2.0 --libs` \
				`pkg-config gthread-2.0 --libs` \
        -lSDL_gfx -lGL -lGLU

common_sources = bullet.c  game.c  physics.c  scenario.c  ship.c  task.c team.c
common_objects = $(common_sources:.c=.o)

all: risc risc-dedicated test_check_collision

%.d: %.c
				@set -e; rm -f $@; \
				$(CC) -M $(CFLAGS) $< > $@.$$$$; \
				sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
				rm -f $@.$$$$

-include $(common_sources:.c=.d)

risc: risc.o particle.o $(common_objects)

particlebench: particlebench.o particle.o $(common_objects)

risc-dedicated: risc-dedicated.o $(common_objects)

test_check_collision: test_check_collision.o physics.o

clean:
	rm -f *.o *.d risc risc-dedicated test_check_collision

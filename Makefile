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

all: gl-viewer recorder

gl-viewer: gl-viewer.o ship.o physics.o bullet.o game.o scenario.o team.o task.o

recorder: recorder.o ship.o physics.o bullet.o game.o scenario.o team.o task.o

test_check_collision: test_check_collision.o physics.o

clean:
	rm -f *.o gl-viewer test_check_collision

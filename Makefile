CFLAGS=-I/usr/include/lua5.1 \
       `sdl-config --cflags` \
       `pkg-config glib-2.0 --cflags` \
       -g -O2 -march=native -Wall

LDFLAGS=-llua5.1 \
        `sdl-config --libs` \
        `pkg-config glib-2.0 --libs` \
        -lSDL_gfx -lGL -lGLU

all: sdl-viewer gl-viewer

sdl-viewer: sdl-viewer.o ship.o physics.o bullet.o game.o

gl-viewer: gl-viewer.o ship.o physics.o bullet.o game.o scenario.o

test_check_collision: test_check_collision.o physics.o

clean:
	rm -f *.o sdl-viewer test_check_collision

CFLAGS=-I/usr/include/lua5.1 \
       `sdl-config --cflags` \
       `pkg-config glib-2.0 --cflags` \
       -g -O2 -march=native -Wall

LDFLAGS=-llua5.1 \
        `sdl-config --libs` \
        `pkg-config glib-2.0 --libs` \
        -lSDL_gfx

all: main

main: main.o ship.o physics.o

clean:
	rm -f *.o main

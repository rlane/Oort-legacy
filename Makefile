SDL_CFLAGS=`pkg-config --cflags sdl`
SDL_LDFLAGS=`pkg-config --libs sdl`

CXXFLAGS=-std=c++0x -g -O1 -I. -Ithird_party/glm

SIM_SRCS=\
	sim/game.cc \
	sim/physics.cc \
	sim/ship.cc

UI_HEADLESS_SRCS=\
	ui/headless/main.cc

UI_SDL_SRCS=\
	ui/sdl/main.cc

all: oort-headless oort-sdl

clean:
	rm -f oort-headless oort-sdl

oort-headless: $(SIM_SRCS) $(UI_HEADLESS_SRCS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

oort-sdl: $(SIM_SRCS) $(UI_SDL_SRCS)
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS) $^ -o $@

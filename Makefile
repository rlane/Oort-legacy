CXXFLAGS=-std=c++0x -g -O1 -I. -Ithird_party/glm

SIM_SRCS=\
	sim/game.cc \
	sim/physics.cc \
	sim/ship.cc

UI_HEADLESS_SRCS=\
	ui/headless/main.cc

all: oort-headless

clean:
	rm -f oort-headless

oort-headless: $(SIM_SRCS) $(UI_HEADLESS_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

RISC
====

Introduction
------------

RISC is a programming game currently in early development. Two or more space
fleets, each ship individually controlled by the player's Lua code, battle in
2-dimensional space. The game simulates Newtonian physics, with the temporary
simplification that ships can thrust and shoot in any direction.

Compilation
-----------

Linux users will need development headers for glib, sdl, sdl\_gfx, opengl, and
lua5.1. Use of luajit is recommended - set the LUA make variable to the name of
your luajit library (luajit-5.1 on Arch).

There are makefiles for OS X and Windows. OS X users will need equivalent
libraries development headers as Linux. The Windows makefile uses mingw to
crosscompile and requires manually downloading appropriate luajit, glib, sdl,
and sdl\_gfx headers. A future task is to document/streamline this process.

Gameplay
--------

To start the game, run risc /path/to/scenario. Omit the scenario argument to
watch a demo scenario.

### Scenarios

The initial configuration of the fleets is controlled by a scenario file, which
is just Lua code with builtin functions to create teams and spawn ships. See
the examples in the scenarios/ directory. Currently the scenario specifies the
Lua file for each ship. This is useful for testing, but in the future this will
likely be changed so that each team has a single AI (which can behave
differently depending on the ship).

### AI

Every ship in the game is controlled by a Lua program that calls functions
provided by RISC to thrust, fire, etc. Each ship is given a timeslice per tick
and preempted when its time is up. Execution resumes where it left off on the
next tick. The ships run in independent Lua VMs and do not share any data. All
coordination must be accomplished using ship orders and the radio.

The best reference for the RISC API is currently the Lua files in the examples/
directory. A summary of the API is given below.

#### RISC API

- position()

- velocity()

- team()

- thrust(angle, force)

- fire(name, angle)

- sensor\_contacts()

- sensor\_contact(id)

- send(msg)

- recv()

- spawn(class, filename, orders)

- yield()

- explode()

- debug\_line(x1, y1, x2, y2)

- clear\_debug\_lines()

### Ships

The available ship classes are specified by the ships.lua file.

Graphical simulator
-------------------

The "risc" binary renders the battle with OpenGL. Currently the graphics are
crude and all ships are represented by circles of the team's color. Missiles
are smaller grey circles. The simulation speed is limited to real time (32 hz).

### Controls

Zoom: scroll wheel, or 'z' and 'x'

Select a ship by clicking on it. This will display various information about
the ship in the lower left corner. If the ship AI has drawn any debug graphics
they will be displayed.

Toggle all debug graphics: 'y'

Pause: space

Single-step: enter

Non-graphical simulator
-----------------------

The "risc-dedicated" binary runs the simulation and outputs which team won. It
isn't framerate-limited and so can run much more quickly. It currently links
against OpenGL anyway, this is a bug. A future task is to have risc-dedicated
output a recording of the battle that can be replayed in a graphical viewer
later.

Contributing
------------

I haven't set up a mailing list yet, so for now please use the GitHub wiki for
discussion and send me pull requests to submit patches.

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

To start the game, run risc /path/to/scenario /path/to/ai[s]. Omit
the scenario argument to watch a demo scenario. Example command lines:

    risc
    risc scenarios/basic.lua examples/switch.lua examples/switch.lua
    risc scenarios/missile_practice.lua examples/rock.lua

### Scenarios

The initial configuration of the fleets is controlled by a scenario file, which
is just Lua code with builtin functions to create teams and spawn ships. See
the examples in the scenarios/ directory. A scenario is given a number (global
"N") of AI filenames (global "AI") which it can assign to teams.

### Victory condition

The team with the last ship alive (not counting missiles) is the winner.

### AI

Every ship in the game is controlled by a Lua program that calls functions
provided by RISC to thrust, fire, etc. Each ship is given a timeslice per tick
and preempted when its time is up. Execution resumes where it left off on the
next tick. The ships run in independent Lua VMs and do not share any data. All
coordination must be accomplished using ship orders and the radio. The amount
of memory that can be allocated per ship is limited to 1 megabyte; this value
may be changed in the future.

The best reference for the RISC API is currently the Lua files in the examples/
directory. A summary of the API is given below.

#### RISC API

- position() - returns a tuple (x,y).

- velocity() - returns a tuple (vx, vy).

- energy() - return the ship's current energy level.

- team() - returns the name of this ship's team.

- class() - returns the name of this ship's class.

- thrust(angle, force) - start thrusting in the given direction.

- fire(name, angle) - fire the gun named "name" in the given direction.

- sensor\_contacts() - returns a table of all the sensor contacts in range.

- sensor\_contact(id) - given an id from from sensor\_contacts, return just that contact.

- send(msg) - broadcast a message to all friendly ships.

- recv() - receive the next message off the queue.

- spawn(class, filename, orders) - spawn a ship.

- yield() - deschedule the program until the next tick.

- explode() - self-destruct.

- debug\_line(x1, y1, x2, y2)

- clear\_debug\_lines()

- orders - a string global containing the orders for this ship as set by the
           spawn() function.

### Ships

The available ship classes are specified by the ships.lua file.

#### Energy

Every ship has an energy supply with a certain recharge rate and a limited
capacity. Energy is used to fire guns and spawn ships. If a ship attempts an
action without having the required energy it is ignored. The ship classes vary
in their energy characteristics; for example, motherships have a large energy
supply that regenerates quickly while missiles have a small energy supply that
does not regenerate at all.

#### Thrust

The thrust() function takes an angle and acceleration. The maximum value of the
acceleration depends on the ship class. The engine will accelerate the ship
using these values until the next time thrust() is called.

#### Spawning

A ship can call the spawn() function to create a new ship. This costs a large
amount of energy that depends on the specified class. Missiles are just another
class of ships in RISC, so to launch a missile simply spawn it with orders of
where to go.

#### Guns

Each ship has zero or more guns determined by its class. The guns are named,
and you pass this name to the fire() function. Guns have varying bullet
masses, bullet velocities, bullet lifetimes, reload times, and energy
costs to fire. The gun's bullet velocity is added to the ship velocity.
Whenever a bullet impacts a ship it does damage equal to its kinetic
energy relative to the ship.

#### Hull

Hull strength varies among ship classes. When a bullet damages a ship its
relative kinetic energy is subtracted from the ship's hull strength. If a
ship's hull strength falls below zero it is destroyed.

#### Self-destruct

The explode() function causes the ship to self-destruct. An explosion (spray of
bullets) is created and the ship is destroyed. The characteristics of the
explosion depend on the ship class, but it is important to note that because
the shrapnel velocities are added to the ship velocity a fast-moving ship will
have a more conical shrapnel pattern. Calling explode() should be rare for most
ship classes, but it is the typical behavior for missiles.

#### Radio

A simple broadcast radio is currently implemented. Sending a message on the
radio costs energy equal to the number of bytes sent. Calling the recv()
function returns the next message received by the ship, or nil if the queue
is empty. Multicast and radio jamming are future development items.

#### Sensors

The sensor\_contacts() function returns a table of all the ships detected by
this ships sensors. This is currently all other ships in the battle, but this
will change to be all ships within a certain range. Each contact is table with
the keys id, team, class, x, y, vx, vy. Most of these are self explanatory. The
id field can be passed to the sensor\_contact() function to return just the
information for the given ship, which is significantly more efficient.

Graphical simulator
-------------------

The "risc" binary renders the battle with OpenGL. The simulation speed is
limited to real time (32 hz).

### Controls

Zoom: scroll wheel, or 'z' and 'x'

Select a ship by clicking on it. This will display various information about
the ship in the lower left corner. If the ship AI has drawn any debug graphics
they will be displayed.

Toggle all debug graphics: 'y'

Pause: space

Single-step: enter

Save screenshot to "screenshot.tga": 'p'

Non-graphical simulator
-----------------------

The "risc-dedicated" binary runs the simulation and outputs which team won. It
isn't framerate-limited and so can run much more quickly. It currently links
against OpenGL anyway, this is a bug. A future task is to have risc-dedicated
output a recording of the battle that can be replayed in a graphical viewer
later.

Contributing
------------

Mailing list: risc-game-dev@lists.sourceforge.net.

Subscribe at [https://lists.sourceforge.net/lists/listinfo/risc-game-dev](https://lists.sourceforge.net/lists/listinfo/risc-game-dev).

Screenshot
----------

![RISC screenshot](risc/wiki/screenshot-1.png)

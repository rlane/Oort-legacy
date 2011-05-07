Oort
====

Introduction
------------

Oort is a programming game currently in early development. Two or more space
fleets, each ship individually controlled by the player's Lua code, battle in
2-dimensional space with Newtonian physics.

Compilation
-----------

Oort uses the standard autotools build system. It is known to compile on Linux
and OS X, and cross-compile to Windows using mingw32. Required packages for
various package managers are listed below:

- Arch Linux: glib2 gtk2 gtkglext glew pkgconfig vala

- Ubuntu 10.10: build-essential valac pkg-config libglib2.0-dev libglew1.5-dev libgtk2.0-dev libgtkglext1-dev

- Arch Linux/mingw32: mingw32-gcc mingw32-glib2 mingw32-gtk2 mingw32-gtkglext mingw32-glew zip vala

- OS X MacPorts: gcc45 glew gtk2 gtkglext glib2-devel

Gameplay
--------

Oort is a programming game, which means that after the simulation has begun the
players have no control over the outcome. Instead, you play by writing a
program (AI) that all ships on your team will individually execute.

To get started, run the UI (`oort_ui` from the command line, or using the
desktop shortcut on Windows). Oort will begin playing a demo scenario. Click
the "File" menu and select "New Game". This will pop up a file chooser where
you will pick the scenario. The file chooser starts in a directory containing
scenarios distributed with the game. Pick "basic.json". Next you'll be prompted
to select two AIs to participate in the battle. Click the file chooser buttons
and select "switch.lua" for each. Now click "OK" to begin the simulation.

### Scenarios

The initial configuration of the fleets is specified by a JSON scenario file. A
description of the format is given below:

    name: (string) Name of the scenario.
    description: (string) Short text describing the scenario.
    author: (string) Name of the author.
    teams: (array of objects):
      name: (string) Team name.
      color: (object):
        r: (integer) Red component 0-255.
        g: (integer) Green component 0-255.
        b: (integer) Blue component 0-255.
      ships: (array of objects):
        class: (string) Ship class name.
        x: (number) X coordinate.
        x: (number) Y coordinate.
        h: (number) Heading in radians.

See the `scenarios/` directory in the distribution for examples. The
`scenarios/challenges/` directory has scenarios that pit a user AI
against progressively more difficult opponents.

### Victory condition

The team with the last ship alive (not counting missiles) is the winner.

### AI

Every ship in the game is controlled by a Lua program that calls functions
provided by Oort to thrust, fire, etc. Each ship is given a timeslice per tick
and preempted when its time is up. Execution resumes where it left off on the
next tick. The ships run in independent Lua VMs and do not share any data. All
coordination must be accomplished using ship orders and the radio. The amount
of memory that can be allocated per ship is limited to 1 megabyte. See the
examples/ directory in the distribution for sample AI.

#### Oort API

- `position()` - returns `(x,y)`.

- `velocity()` - returns `(vx,vy)`.

- `energy()` - return the ship's current energy level.

- `thrust_main(acc)` - set the main thruster to produce the given acceleration.

- `thrust_lateral(acc)` - set the lateral thrusters to produce the given acceleration.

- `thrust_angular(acc)` - set the angular acceleration.

- `fire(name, angle)` - fire the gun named `name` in the given direction.

- `sensor_contacts(query=nil)` - returns a table of all the sensor contacts
matching `query`. See the "Sensors" section for full details.

- `sensor_contact(id)` - given an id from from `sensor_contacts`, return just that contact.

- `send(msg)` - broadcast a message to all friendly ships.

- `recv()` - receive the next message from the radio. Returns `nil` if there are no
messages on the queue.

- `spawn(class, filename, orders)` - spawn a ship.

- `yield()` - deschedule the program until the next tick.

- `explode()` - self-destruct.

- `debug_line(x1, y1, x2, y2)` - Draw a line in world coordinates for visual debugging.

- `clear_debug_lines()` - Erase all debug lines.

- `orders` - a string global containing the orders for this ship as set by the
spawn() function.

- `class` - a string global containing this ship's class name.

- `team` - a string global containing this ship's team's name.

- `ships` - a global table containing all the properties of each ship class,
keyed by class name.

The standard `math`, `table`, and `string` libraries are provided. A library of
useful utility functions (`lib.lua`) is also included in the global
environment. The utility functions include a standard missile AI which is
useful for beginning players.

### Ships

The available ship classes are specified by the ships.lua file. This table is
available to the AI in the `ships` global. The properties of existing ships can
be changed by editing this file, but adding new ships requires adding
corresponding code in the renderer.

#### Energy

Every ship has an energy supply with a certain recharge rate and a limited
capacity. Energy is used to fire guns, spawn ships, and thrust. If a ship
attempts an action without having the required energy it is ignored. The ship
classes vary in their energy characteristics; for example, carriers have a
large energy supply that regenerates quickly while missiles have a small energy
supply that does not regenerate at all.

#### Thrust

Each ship has three sets of thrusters: main, lateral, and angular. The main
thrusters operate parallel to the ship's heading while the lateral thrusters
are perpendicular. The maximum accelerations for each thruster are defined by
the ship's class. The engines will continue applying the given acceleration
until it is changed by a call to a thrust function or the ship's energy runs
out.

#### Spawning

A ship can call the spawn() function to create a new ship. This costs a large
amount of energy that depends on the specified class. Missiles are just another
class of ships in Oort, so to launch a missile simply spawn it with orders of
where to go. The classes of ships that can be spawned are controlled by the
'spawnable' fields in ships.lua.

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
ship's hull strength reaches zero it is destroyed.

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

The `sensor_contacts()` function returns a table of all the ships detected by
this ships sensors. Each contact is a table with the fields `id`, `team`,
`class`, `x`, `y`, `vx`, `vy`.  Most of these are self explanatory. The `id`
field is an opaque string that can be passed to the `sensor_contact()` function
to return just the information for the given ship, which is significantly more
efficient.

There is an optional `query` argument to `sensors_contact()`. A sensor query is
a table whose format is given below:

    enemy: (boolean) True to match only enemy ships, or false to match only friendly ships.
    class: (string) Match only ships of the given class.
    distance_lt: (number) Match only ships closer than the given distance.
    distance_gt: (number) Match only ships further away than the given distance.
    hull_lt: (number) Match only ships whose hull strength is less than the given amount.
    hull_gt: (number) Match only ships whose hull strength is greater than the given amount.
    limit: (integer) Return at most the given number of results.

If a query option is omitted it has no effect. The ships returned by the query
will be exactly those that match all of the given query options.

### Determinism

The simulation is designed to be deterministic. Given the same scenario, AI,
and random seed it should play out in exactly the same way every time. There
are some limitations to this: programs that exceed their timeslice or run out
of memory will not behave identically across different Lua VMs (standard Lua vs
LuaJIT). They should still be deterministic given the same VM.

Graphical simulator
-------------------

The "oort\_ui" binary renders the battle with OpenGL. The simulation speed is
limited to real time (32 hz).

### Controls

Zoom: scroll wheel, or 'z' and 'x'

Toggle all debug graphics: 'y'

Pause: space

Single-step: enter

Take a screenshot: 'p'

Toggle FPS display: 'f'

Toggle following picked ship: 'v'

Toggle player control of picked ship: 'o'

You can click on a ship to "pick" it. Data about the currently picked ship is
shown in the lower-left corner of the display, and any debug lines this ship
has drawn will be shown.

Just for fun, you can take over your currently picked ship with the 'o' key.
The keys 'ws', 'ad', and 'jl' control the thrusters and 'i' fires the main gun.

Non-graphical simulator
-----------------------

The "oort\_dedicated" binary runs the simulation and outputs which team won. It
isn't framerate-limited and so can run much more quickly. A future task is to
have oort\_dedicated output a recording of the battle that can be replayed in a
graphical viewer later.

Contributing
------------

Mailing list: oort-game-dev@lists.sourceforge.net.

Subscribe at [https://lists.sourceforge.net/lists/listinfo/oort-game-dev](https://lists.sourceforge.net/lists/listinfo/oort-game-dev).

Screenshot
----------

![Oort screenshot](Oort/wiki/screenshot-1.png)

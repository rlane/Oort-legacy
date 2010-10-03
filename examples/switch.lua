local c = class()

if c == "fighter" then dofile("examples/orbit.lua")
elseif c == "mothership" then dofile("examples/rock.lua")
elseif c == "missile" then dofile("examples/missile.lua")
elseif c == "little_missile" then dofile("examples/little_missile.lua")
else dofile("examples/target.lua")
end

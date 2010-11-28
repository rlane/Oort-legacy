local c = class()

if c == "fighter" then dofile(data_dir .. "/examples/orbit.lua")
elseif c == "mothership" then dofile(data_dir .. "/examples/rock.lua")
elseif c == "missile" then dofile(data_dir .. "/examples/missile.lua")
elseif c == "little_missile" then dofile(data_dir .. "/examples/little_missile.lua")
else dofile(data_dir .. "/examples/target.lua")
end

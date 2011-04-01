local my_class = class
local my_ship = ships[my_class]

if class == "fighter" then
	while true do
		local t = sensor_contacts{ enemy=true }[1]
		local tx, ty = t:position()
		turn_towards(tx,ty)
		if t then
			spawn("little_missile", t:id())
		end
		yield()
	end
elseif class == "little_missile" then
	standard_missile_ai()
end

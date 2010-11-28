dofile(data_dir .. "/examples/lib.lua")

local target_id = deserialize_id(orders)

thrust(math.random()*2*math.pi, 5)
sleep(16)

while true do
	local t = sensor_contact(target_id)

	if not t then
		--printf("lost target\n")
		thrust(0, 0)
		sleep(64)
		explode()
	end

	local tx, ty = t:position()
	local tvx, tvy = t:velocity()

	clear_debug_lines()
	debug_diamond(tx, ty, 0.3)
	
	local x, y = position()
	if distance(tx, ty, x, y) < 3 then
		explode()
	end

	local vx, vy = velocity()
	local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, 30, math.huge)
	if a then
		thrust(a, 5)
	else
		explode()
	end

	yield()
end

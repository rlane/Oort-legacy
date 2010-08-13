dofile("examples/lib.lua")

local target_id = orders

thrust(math.random()*2*math.pi, 5)
sleep(16)

while true do
	t = sensor_contact(target_id)

	if not t then
		--printf("lost target\n")
		thrust(0, 0)
		sleep(64)
		explode()
	end

	clear_debug_lines()
	debug_diamond(t.x, t.y, 0.3)
	
	local x, y = position()
	if distance(t.x, t.y, x, y) < 3 then
		explode()
	end

	local vx, vy = velocity()
	local a = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, 30, math.huge)
	if a then
		thrust(a, 5)
	else
		explode()
	end

	yield()
end

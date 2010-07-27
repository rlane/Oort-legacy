dofile("examples/lib.lua")

local x, y = position()

thrust(math.random()*2*math.pi, 5)
sleep(16)
local target_selector = function(k,c) return c.team == enemy_team() and c.class ~= "missile" end
local target_id, t = pick(sensor_contacts(), target_selector)

if not t then
	printf("no target\n")
	sleep(64)
	explode()
end

while true do
	t = sensor_contacts()[target_id]

	if not t then
		printf("lost target\n")
		sleep(64)
		explode()
	end
	
	local x, y = position()
	if distance(t.x, t.y, x, y) < 3 then
		explode()
	end

	local vx, vy = velocity()
	local a = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, 30, math.huge)
	if a then
		thrust(a, 10)
	else
		explode()
	end

	yield()
end

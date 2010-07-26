dofile("examples/lib.lua")

local x, y = position()

thrust(math.random()*2*math.pi, 5)
sleep(16)

local t = pick_close_enemy(x, y, enemy_team(), 100, 1)
if not t then
	explode()
end

while true do
	local x, y = position()
	local vx, vy = velocity()
	
	if distance(t.x, t.y, x, y) < 3 then
		explode()
	end

	local a = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, 30, math.huge)
	if a then
		thrust(a, 10)
	else
		explode()
	end

	yield()
end

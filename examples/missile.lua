dofile("examples/lib.lua")

local x, y = position()

thrust(math.random()*2*math.pi, 5)
sleep(16)

local enemy_contacts = selecti(sensor_contacts(), function(c) return c.team == enemy_team() and c.class ~= "missile" end)
local n = table.maxn(enemy_contacts)
if n == 0 then explode() end
local t = enemy_contacts[math.random(n)]

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

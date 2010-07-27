dofile("examples/lib.lua")

local x, y = position()

thrust(math.random()*2*math.pi, 5)
sleep(16)

local contacts = sensor_contacts()
local enemy_contacts = select(contacts, function(k,c) return c.team == enemy_team() and c.class ~= "missile" end)
local enemy_ids = keys(enemy_contacts)
local n = table.maxn(enemy_ids)
if not n then explode() end
local target_id = enemy_ids[math.random(n)]

while true do
	local x, y = position()
	local vx, vy = velocity()
	local contacts = sensor_contacts()
	local t = contacts[target_id]

	if not t then
		printf("lost target\n")
		sleep(64)
		explode()
	end
	
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

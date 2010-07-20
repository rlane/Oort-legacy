dofile("lib.lua")

thrust(math.pi/2, 1)
sleep(32)

local i = 0
local t = nil
local bullet_lifetime = 1
local bullet_speed = 10
local max_target_distance = bullet_speed*bullet_lifetime
local orbit_x = 0
local orbit_y = 0

while true do
	if i == 0 then
		local x, y = position()
		t = pick_close_enemy(x, y, enemy_team(), max_target_distance, 0.8)
	end

	i = i + 1
	if i == 32 then
		i = 0
	end

	local x, y = position()
	local vx, vy = velocity()

	if t then
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, bullet_speed, bullet_lifetime)
		if a2 then
			fire(a2)
		end
	end

	local a = angle_between(x, y, orbit_x, orbit_y)
	local f = math.min(10, 20.0/(distance(x, y, orbit_x, orbit_y)^2))
	thrust(a, f)

	yield()
end

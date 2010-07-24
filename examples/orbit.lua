dofile("examples/lib.lua")

thrust(math.pi/2, 1)
sleep(32)

local i = math.random(1,16)
local t = nil
local bullet_lifetime = 1
local bullet_speed = 20
local max_target_distance = bullet_speed*bullet_lifetime
local orbit_x = 0
local orbit_y = 0

while true do
	if i == 0 then
		local x, y = position()
		t = pick_close_enemy(x, y, enemy_team(), max_target_distance*1.5, 0.8)
	end

	i = i + 1
	if i >= 16 then
		i = 0
	end

	local x, y = position()
	local vx, vy = velocity()

	if t then
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, bullet_speed, bullet_lifetime)
		local d = distance(x, y, t.x, t.y)
		if a2 then
			fire("main", a2)
		end

		orbit_x = t.x
		orbit_y = t.y
	else
		orbit_x = 0
		orbit_y = 0
	end

	local a = angle_between(x, y, orbit_x, orbit_y)
	local k = math.random(10)
	if k < 7 then
		local f = math.min(10, 10.0/math.sqrt(distance(x, y, orbit_x, orbit_y)))
		thrust(a, f)
	elseif k < 8 then
		a = normalize_angle(a + R(0.5,1) * sign(R(-1,1)))
		thrust(a, 5)
	else
		a = normalize_angle(a + (math.pi/2) * sign(R(-1,1)))
		thrust(a, 5)
	end

	yield()
end

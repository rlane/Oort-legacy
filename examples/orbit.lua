dofile("examples/lib.lua")

thrust(math.pi/2, 1)
sleep(32)

local i = math.random(1,16)
local t = nil
local bullet_lifetime = 0.5
local bullet_speed = 40
local max_target_distance = bullet_speed*bullet_lifetime
local orbit_x = 0
local orbit_y = 0

while true do
	local msg = recv()
	if msg then
		print("msg: " .. msg)
	end

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
			fire("main", a2+R(-0.04,0.04))
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
		local f = math.min(5, 1.0*math.sqrt(distance(x, y, orbit_x, orbit_y)))
		local nvx = vx + f * math.cos(a)
		local nvy = vy + f * math.sin(a)
		if distance(0, 0, nvx, nvy) < 10 then
			thrust(a, f)
		end
	elseif k < 8 then
		a = normalize_angle(a + R(0.5,1) * sign(R(-1,1)))
		thrust(a, 5)
	else
		a = normalize_angle(a + (math.pi/2) * sign(R(-1,1)))
		thrust(a, 5)
	end

	yield()
end

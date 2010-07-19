dofile("lib.lua")

if true then
thrust(0, 10)
sleep(32)
thrust(0, -10)
sleep(32)
thrust(0, 0)

sleep(32)

thrust(math.pi/2, 1)
sleep(32)
end

local i = 0
local t = nil
local bullet_lifetime = 1
local bullet_speed = 10
local max_target_distance = bullet_speed*bullet_lifetime

while true do
	if i == 0 then
		local x, y = position()
		t = pick_closest_enemy(x, y, "blue", max_target_distance)
	end

	i = i + 1
	if i == 32 then
		i = 0
	end

	local x, y = position()
	local vx, vy = velocity()

	if t then
		local a = angle_between(x, y, t.x, t.y)
		thrust(a, 20.0/(distance(x, y, t.x, t.y)^2))
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, bullet_speed, bullet_lifetime)
		if a2 then
			fire(a2)
		end
	else
		local a = angle_between(x, y, 0, 0)
		thrust(a, 20.0/(distance(x, y, 0, 0)^2))
	end

	yield()
end

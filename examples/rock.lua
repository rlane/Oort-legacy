dofile("examples/lib.lua")

local i = 0
local t = nil
local bullet_lifetime = 5
local bullet_speed = 10
local max_target_distance = bullet_speed*bullet_lifetime

while true do
	local x, y = position()
	local vx, vy = velocity()

	t = pick_close_enemy(x, y, enemy_team(), max_target_distance, 0.5)

	if t then
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, bullet_speed, bullet_lifetime)
		if a2 then
			fire(a2+R(-0.1,0.1))
		end
	end

	yield()
end
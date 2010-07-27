dofile("examples/lib.lua")

local i = 0
local t = nil

local main_bullet_lifetime = 10
local main_bullet_speed = 5

local flak_bullet_lifetime = 0.5
local flak_bullet_speed = 25

while true do
	local msg = recv()
	if msg then
		print("msg: " .. msg)
	end

	local x, y = position()
	local vx, vy = velocity()
	t = pick_close_enemy(x, y, enemy_team(), main_bullet_speed*main_bullet_lifetime, 0.5)

	if t then
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, main_bullet_speed, main_bullet_lifetime)
		if a2 then fire("main", a2) end
	end

	for i = 1,3 do
		local x, y = position()
		local vx, vy = velocity()
		t = pick_close_enemy(x, y, enemy_team(), flak_bullet_speed*flak_bullet_lifetime, 0.3)

		if t then
			local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, flak_bullet_speed, flak_bullet_lifetime)
			if a2 then
				local spread = 0.1
				fire("flak" .. i, a2+R(-spread,spread))
			end
		end
	end

	if math.random(1,1000) == 5 then
		--send("hello")
	end

	if math.random(1,100) == 7 then
		spawn("missile", "examples/missile.lua")
	end

	if math.random(50) == 7 then
		spawn("little_missile", "examples/little_missile.lua")
	end

	if math.random(200) == 7 then
		spawn("fighter", "examples/orbit.lua")
	end

	yield()
end

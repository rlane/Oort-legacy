dofile("examples/lib.lua")

local i = 0
local t = nil

local main_bullet_lifetime = 10
local main_bullet_speed = 5

local flak_bullet_lifetime = 0.5
local flak_bullet_speed = 25

local main_target = nil
local main_target_retry = 0

local flak_target = nil
local flak_target_retry = 0

while true do
	local msg = recv()
	if msg then
		print("msg: " .. msg)
	end

	if not main_target and main_target_retry == 16 then
		local x, y = position()
		local vx, vy = velocity()
		main_target = pick_close_enemy(x, y, enemy_team(), main_bullet_speed*main_bullet_lifetime, 0.5)
		main_target_retry = 0
	elseif not main_target then
		main_target_retry = main_target_retry + 1
	else
		main_target = sensor_contact(main_target.id)
	end

	if main_target then
		local x, y = position()
		local vx, vy = velocity()
		local t = main_target
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, main_bullet_speed, main_bullet_lifetime)
		if a2 then
			fire("main", a2)
		else
			main_target = nil
		end
	end

	if not flak_target and flak_target_retry == 16 then
		local x, y = position()
		local vx, vy = velocity()
		flak_target = pick_close_enemy(x, y, enemy_team(), flak_bullet_speed*flak_bullet_lifetime, 0.3)
	elseif not flak_target then
		flak_target_retry = flak_target_retry + 1
	else
		flak_target = sensor_contact(flak_target.id)
	end

	if flak_target then
		local x, y = position()
		local vx, vy = velocity()
		local t = flak_target
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, flak_bullet_speed, flak_bullet_lifetime)
		if a2 then
			local spread = 0.1
			for i = 1,3 do
				fire("flak" .. i, a2+R(-spread,spread))
			end
		else
			flak_target = nil
		end
	end

	if math.random(1,1000) == 5 then
		--send("hello")
	end

	if math.random(1,100) == 7 then
		spawn("missile", "examples/missile.lua", "")
	end

	if math.random(50) == 7 then
		spawn("little_missile", "examples/little_missile.lua", "")
	end

	if math.random(200) == 7 then
		spawn("fighter", "examples/orbit.lua", "foo")
	end

	yield()
end

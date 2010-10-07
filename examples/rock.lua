dofile("examples/lib.lua")

local my_team = team()
local my_ship = ships[class()]

local i = 0
local t = nil

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
		main_target = pick_close_enemy(x, y, my_ship.guns.main.bullet_velocity*my_ship.guns.main.bullet_ttl, 0.5)
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
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, my_ship.guns.main.bullet_velocity, my_ship.guns.main.bullet_ttl)
		if a2 then
			fire("main", a2)
		else
			main_target = nil
		end
	end

	if my_ship.guns.flak1 then
		if not flak_target and flak_target_retry == 16 then
			local x, y = position()
			local vx, vy = velocity()
			flak_target = pick_close_enemy(x, y, my_ship.guns.flak1.bullet_velocity*my_ship.guns.flak1.bullet_ttl, 0.3)
		elseif not flak_target then
			flak_target_retry = flak_target_retry + 1
		else
			flak_target = sensor_contact(flak_target.id)
		end

		if flak_target then
			local x, y = position()
			local vx, vy = velocity()
			local t = flak_target
			local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, my_ship.guns.flak1.bullet_velocity, my_ship.guns.flak1.bullet_ttl)
			if a2 then
				local spread = 0.1
				for i = 1,3 do
					fire("flak" .. i, a2+R(-spread,spread))
				end
			else
				flak_target = nil
			end
		end
	end

	if math.random(1,1000) == 5 then
		--send("hello")
	end

	if math.random(1,100) == 7 then
		local target_selector = function(k,c) return c.team ~= my_team and c.class == "mothership" end
		local target_id, t = pick(sensor_contacts(), target_selector)
		if target_id then
			spawn("missile", "examples/missile.lua", target_id)
		end
	end

	if math.random(50) == 7 then
		local target_selector = function(k,c) return c.team ~= my_team and c.class ~= "little_missile" end
		local target_id, t = pick(sensor_contacts(), target_selector)
		if target_id then
			spawn("little_missile", "examples/little_missile.lua", target_id)
		end
	end

	if math.random(200) == 7 then
		spawn("fighter", "examples/orbit.lua", "foo")
	end

	yield()
end

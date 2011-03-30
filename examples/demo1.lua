local my_class = class
local my_team = team
local my_ship = ships[my_class]
local fighter_selector = function(k,c) return c:team() ~= my_team and c:class() == "fighter" end
local missile_selector = function(k,c)
	if c:team() ~= my_team and c:class() == "little_missile" then
		local x, y = position()
		local tx, ty = c:position()
		return distance(x,y,tx,ty) < 600
	else
		return false
	end
end

if my_class == "fighter" then
	for i = 1,256 do
		turn_towards(0,0)
		yield()
	end

	while true do
		local x, y = position()
		local dist = distance(0,0,x,y)

		turn_towards(0,0)
		thrust_main(30)

		if dist < 1000 then
			thrust_lateral(10)
		else
			thrust_lateral(0)
		end

		local _, t = pick(sensor_contacts{}, missile_selector)
		if t then
			local vx, vy = velocity()
			local tx, ty = t:position()
			local tvx, tvy = t:velocity()
			local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.bullet_velocity, my_ship.guns.main.bullet_ttl)
			if a then
				fire("main", a)
			end
		end

		yield()
	end
elseif my_class == "mothership" then
	local i = 0
	while true do
		local x, y = position()
		local dist = distance(0,0,x,y)

		turn_towards(0,0)
		thrust_main(5)

		if dist < 400 then
			thrust_lateral(5.0/3)
		else
			thrust_lateral(0)
		end

		if energy() > my_ship.energy.limit*0.1 and math.random(100) == 1 then
			for i = 1,4 do
				local _, t = pick(sensor_contacts{}, fighter_selector)
				if t then
					spawn("little_missile", t:id())
				end
			end
		end

		yield()
	end
elseif my_class == "little_missile" then
	local target_id = orders
	local start_angle = math.random()*2*math.pi
	for i = 1,16 do
		turn_to(start_angle)
		yield()
	end

	while true do
		local t = sensor_contact(target_id)

		if not t then
			sleep(64)
			explode()
		end

		local x, y = position()
		local vx, vy = velocity()
		local tx, ty = t:position()
		local tvx, tvy = t:velocity()

		clear_debug_lines()
		debug_diamond(tx, ty, 16*my_ship.radius)
		drive_towards(tx,ty,500)

		local ttt = distance(x, y, tx, ty) / (distance(vx, vy, tvx, tvy))
		if ttt < 0.5 then
			explode()
		end

		yield()
	end
else
	explode()
end

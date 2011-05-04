local my_class = class
local my_ship = ships[my_class]

if my_class == "fighter" then
	local i = math.random(1,256)
	local t = nil
	local max_target_distance = my_ship.guns.main.bullet_velocity*my_ship.guns.main.bullet_ttl*1.5
	local origin = { x = 0, y = 0, vx = 0, vy = 0 }
	local target_selector = function(k,c) return c:class() ~= "little_missile" end
	local follow_target = nil
	local follow_target_retry = 0
	local fire_target = nil
	local fire_target_retry = 0

	local function fire_score(c)
		local x,y = position()
		if c:id() == target_id then
			return 0
		else
			local a = angle_between(x,y,c:position())
			return math.abs(angle_diff(heading(), a))
		end
	end

	while true do
		local msg = recv()
		if msg then
			--print("fighter msg: " .. msg)
		end

		clear_debug_lines()
		local x, y = position()
		local vx, vy = velocity()

		if not follow_target and follow_target_retry == 16 then
			local contacts = sensor_contacts{ enemy = true }
			_, follow_target = pick(contacts, target_selector)
		elseif not follow_target then
			follow_target_retry = follow_target_retry + 1
		else
			follow_target = sensor_contact(follow_target:id())
		end

		local follow
		if follow_target and distance(x,y,0,0) < 3000 then
			follow = follow_target
		else
			follow = nil
		end

		local follow_x, follow_y
		local follow_vx, follow_vy
		if follow == nil then
			follow_x, follow_y = 0, 0
			follow_vx, follow_vy = 0, 0
		else
			follow_x, follow_y = follow:position()
			follow_vx, follow_vy = follow:velocity()
		end
		debug_square(follow_x, follow_y, 2*my_ship.radius)

		local urgent_target = 
			      min_by(sensor_contacts{ distance_lt = max_target_distance, class = "missile", enemy = true }, fire_score) or
			      min_by(sensor_contacts{ distance_lt = max_target_distance, class = "little_missile", enemy = true }, fire_score)
		if urgent_target then fire_target = urgent_target end

		if not fire_target and fire_target_retry >= 16 then
			fire_target = min_by(sensor_contacts{ distance_lt = max_target_distance, enemy = true }, fire_score)
			fire_target_retry = 0
		elseif not fire_target then
			fire_target_retry = fire_target_retry + 1
		else
			fire_target = sensor_contact(fire_target:id())
		end

		if fire_target then
			local tx, ty = fire_target:position()
			local tvx, tvy = fire_target:velocity()
			debug_diamond(tx, ty, my_ship.radius)
			local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.bullet_velocity, my_ship.guns.main.bullet_ttl)
			if a then
				fire("main", a)
			else
				fire_target = nil
			end
			--debug_box(t.x-1, t.y-1, t.x+1, t.y+1)
		else
			--debug_box_off()
		end

		drive_towards(follow_x, follow_y, my_ship.max_main_acc*3)

		if follow_target and energy() > ships.little_missile.cost and math.random(50) == 7 then
			spawn("little_missile", follow_target:id())
		end

		yield()
	end
elseif my_class == "ion_cannon_frigate" then
	local main_target = nil
	local main_target_retry = 0
	local max_dist = my_ship.guns.main.bullet_velocity * my_ship.guns.main.bullet_ttl

	while true do
		clear_debug_lines()

		if not main_target and main_target_retry == 16 then
			local x, y = position()
			local vx, vy = velocity()
			main_target = sensor_contacts{ class = "ion_cannon_frigate", enemy = true }[1] or
                    sensor_contacts{ class = "mothership", enemy = true }[1]
			main_target_retry = 0
		elseif not main_target then
			main_target_retry = main_target_retry + 1
		else
			main_target = sensor_contact(main_target:id())
		end

		if main_target then
			local x, y = position()
			local tx, ty = main_target:position()
			local bearing = angle_between(x, y, tx, ty)
			local da = angle_diff(heading(),bearing)
			local dist = distance(x, y, tx, ty)
			debug_square(tx, ty, 2*my_ship.radius)
			if dist <= max_dist and math.abs(da) < 0.1 then
				fire("main", heading())
			end
			drive_towards(tx, ty, 200);
		else
			drive_towards(0, 0, 100);
		end

		yield()
	end
elseif my_class == "mothership" then
	local i = 0
	local t = nil
	local main_target = nil

	while true do
		local msg = recv()
		if msg then
			--print("mothership msg: " .. msg)
		end

		local range = my_ship.guns.main.bullet_velocity*my_ship.guns.main.bullet_ttl
		main_target = sensor_contacts{ distance_lt = range, enemy = true, class = "missile", limit = 1 }[1] or
                  sensor_contacts{ distance_lt = range, enemy = true, class = "little_missile", limit = 1 }[1]

		if not main_target and energy() > 0.1*my_ship.energy.limit then
			main_target = sensor_contacts{ distance_lt = range, enemy = true, limit = 1 }[1]
		end

		if main_target then
			local x, y = position()
			local vx, vy = velocity()
			local tx, ty = main_target:position()
			local tvx, tvy = main_target:velocity()
			local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.bullet_velocity, my_ship.guns.main.bullet_ttl)
			if a then
				fire("main", a)
			else
				main_target = nil
			end
		end

		if math.random(1,1000) == 5 then
			send("hello")
		end

		if math.random(1,100) == 7 then
			local target_selector = function(k,c) return true end
			local _, t = pick(sensor_contacts{ class = "mothership", enemy = true }, target_selector)
			if t then
				spawn("missile", t:id())
			end
		end

		if math.random(50) == 7 then
			local target_selector = function(k,c) return c:class() ~= "little_missile" end
			local _, t = pick(sensor_contacts{ enemy = true }, target_selector)
			if t then
				spawn("little_missile", t:id())
			end
		end

		if energy() > 100 and math.random(50) == 7 then
			spawn("fighter", "")
		end

		yield()
	end
elseif my_class == "missile" or my_class == "little_missile" then
	standard_missile_ai()
end

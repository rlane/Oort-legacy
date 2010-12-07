local my_class = class()
local my_team = team()
local my_ship = ships[my_class]
local fighter_selector = function(k,c) return c:team() ~= team() and c:class() == "fighter" end
local missile_selector = function(k,c)
	if c:team() ~= team() and c:class() == "little_missile" then
		local x, y = position()
		local tx, ty = c:position()
		return distance(x,y,tx,ty) < 3
	else
		return false
	end
end

if my_class == "fighter" then
	local i = 0
	while true do
		local x, y = position()
		local a = angle_between(x, y, 0, 0)
		if i == 0 then
			if distance(x, y, 0, 0) < 10 then
				thrust(a+math.pi/2, 1)
			end
			i = 1
		elseif i == 1 then
			thrust(a, 3)
			i = 0
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
		local a = angle_between(x, y, 0, 0)
		if i == 0 then
			if distance(x, y, 0, 0) < 4 then
				thrust(a-math.pi/2, 1)
			end
			i = 1
		elseif i == 1 then
			thrust(a, 3)
			i = 0
		end

		if energy() == my_ship.energy.limit and math.random(100) == 1 then
			for i = 1,4 do
				local _, t = pick(sensor_contacts{}, fighter_selector)
				if t then
					spawn("little_missile", serialize_id(t:id()))
				end
			end
		end

		yield()
	end
elseif my_class == "little_missile" then
	local target_id = deserialize_id(orders)
	thrust(math.random()*2*math.pi, my_ship.max_acc)
	sleep(16)

	while true do
		local t = sensor_contact(target_id)

		if not t then
			thrust(0, 0)
			sleep(64)
			explode()
		end

		local tx, ty = t:position()
		local tvx, tvy = t:velocity()

		clear_debug_lines()
		debug_diamond(tx, ty, 16*my_ship.radius)
		
		local x, y = position()
		local vx, vy = velocity()

		local ttt = distance(x, y, tx, ty) / (distance(vx, vy, tvx, tvy)+my_ship.explosion.velocity)
		if ttt < 0.1 then
			explode()
		end

		local v = distance(0, 0, vx, vy)
		local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.max_acc+v, math.huge)
		if a then
			thrust(a, my_ship.max_acc)
		else
			explode()
		end

		yield()
	end
else
	explode()
end

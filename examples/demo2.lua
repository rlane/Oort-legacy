local my_class = class
local my_ship = ships[my_class]

if class == "carrier" then
	local main_target = nil
	local dest = nil
	while true do
		if not main_target then
			local x, y = position()
			local vx, vy = velocity()
			main_target = pick_close_enemy(x, y, my_ship.guns.main.velocity*my_ship.guns.main.ttl, 0.5)
		else
			main_target = sensor_contact(main_target:id())
		end

		if main_target then
			local x, y = position()
			local vx, vy = velocity()
			local tx, ty = main_target:position()
			local tvx, tvy = main_target:velocity()
			local a2 = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.velocity, my_ship.guns.main.ttl)
			if a2 then
				fire("main", a2)
			else
				main_target = nil
			end
		end

		if dest then
			local x,y = position()
			local dx = dest_half - x
			if dx > 0 then thrust(0, 4)
			else thrust(0, -4) end
			local vx,vy = velocity()
			if math.abs(x-dest) < 1 and vx < 0.1 then
				dest = nil
				thrust(0,0)
			end
		else
			dest = math.random(40) - 20
			local x,y = position()
			local dx = dest - x
			dest_half = x + dx/2
		end

		yield()
	end
elseif class == "fighter" then
	local epsilon = 0.001

	function assert_stopped()
		vx,vy = velocity()
		assert(math.abs(vx) < epsilon)
		assert(math.abs(vy) < epsilon)
	end

	function assert_stopped_y()
		vx,vy = velocity()
		assert(math.abs(vy) < epsilon)
	end

	local acc = 4
	local down_acc = 3
	local thrust_time = 32
	local down_thrust_time = 48
	local sleep_time = 256
	assert_stopped()
	thrust(0,-acc/2)
	sleep(thrust_time)
	thrust(0,0)
	sleep(sleep_time/2)
	while true do
		assert_stopped_y()
		thrust(0,acc)
		sleep(thrust_time/2)
		assert_stopped()
		thrust(math.pi/2, down_acc)
		sleep(down_thrust_time/2)
		thrust(math.pi/2, -down_acc)
		sleep(down_thrust_time/2)
		assert_stopped()
		local x,y = position()
		if y > 8 then break end
		thrust(0,acc)
		sleep(thrust_time/2)
		assert_stopped_y()
		thrust(0,0)
		sleep(sleep_time)
		assert_stopped_y()
		acc = -acc
	end

	local t = sensor_contacts{enemy=true, class="carrier"}[1]
	while true do
		t = sensor_contact(t:id())
		local x,y = position()
		local tx,ty = t:position()
		local a = angle_between(x,y,tx,ty)
		thrust(a, 4)
		if distance(x,y,tx,ty) < 3 then
			explode()
		end
	end
end

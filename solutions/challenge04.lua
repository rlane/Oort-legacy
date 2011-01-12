local my_class = class
local my_ship = ships[my_class]

if class == "fighter" then
	while true do
		local t = sensor_contacts{ enemy=true }[1]
		if t then
			spawn("little_missile", t:id())
		end
		yield()
	end
elseif class == "little_missile" then
	local N = my_ship.max_acc*1000
	local t = sensor_contact(orders)
	local last_bearing = 0
	if not t then explode() end
	for i = 1,16 do
		local x, y = position()
		local tx, ty = t:position()
		last_bearing = angle_between(x, y, tx, ty)
		thrust(last_bearing, my_ship.max_acc)
		yield()
	end
	while true do
		t = sensor_contact(orders)
		if not t or energy() == 0 then explode() end
		x, y = position()
		vx, vy = velocity()
		tx, ty = t:position()
		local v_angle = angle_between(0, 0, vx, vy)
		local bearing = angle_between(x, y, tx, ty)
		local bearing_diff = bearing - last_bearing
		local acc = N*bearing_diff
		local accx = acc*math.cos(v_angle+math.pi/2) + my_ship.max_acc*math.cos(v_angle)
		local accy = acc*math.sin(v_angle+math.pi/2) + my_ship.max_acc*math.sin(v_angle)
		local thrust_angle = angle_between(0, 0, accx, accy)
		thrust(thrust_angle, my_ship.max_acc)
		last_bearing = bearing
		if distance(x,y,tx,ty)/distance(0, 0, vx, vy) < 1/32 then explode() end
		yield()
	end
end

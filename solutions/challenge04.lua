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
	thrust_main(my_ship.max_acc)
	if not t then explode() end
	for i = 1,16 do
		local x, y = position()
		local tx, ty = t:position()
		last_bearing = angle_between(x, y, tx, ty)
		turn_to(last_bearing)
		yield()
	end
	while true do
		t = sensor_contact(orders)
		if not t or energy() == 0 then explode() end
		local x, y = position()
		local vx, vy = velocity()
		local tx, ty = t:position()
		drive_towards(tx,ty,100)
		if distance(x,y,tx,ty)/distance(0, 0, vx, vy) < 1/32 then explode() end
		yield()
	end
end

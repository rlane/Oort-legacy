local my_class = class
local my_ship = ships[my_class]

if class == "fighter" then
	while true do
		local t = sensor_contacts{ enemy=true }[1]
		local tx, ty = t:position()
		turn_towards(tx,ty)
		if t then
			spawn("little_missile", t:id())
		end
		yield()
	end
elseif class == "little_missile" then
	local t = sensor_contact(orders)
	if not t then explode() end
	local x, y = position()
	local tx, ty = t:position()
	local last_bearing = angle_between(x, y, tx, ty)

	while true do
		clear_debug_lines()
		t = sensor_contact(orders)
		if not t or energy() == 0 then explode() end
		local x, y = position()
		local vx, vy = velocity()
		local h = heading()
		local tx, ty = t:position()
		local tvx, tvy = t:velocity()
		local bearing = angle_between(x, y, tx, ty)
		local bearing_rate = angle_diff(bearing, last_bearing)*32.0
		last_bearing = bearing

		local dvx, dvy = vx-tvx, vy-tvy
		local rvx, rvy = rotate(dvx, dvy, -bearing)

		local k = 5
		local v = rvx
		local n = -k*v*bearing_rate

		thrust_main(my_ship.max_acc)
		thrust_lateral(n)
		turn_to(bearing)

		if distance(x,y,tx,ty)/distance(0, 0, vx, vy) < 1/32 then explode() end
		yield()
	end
end

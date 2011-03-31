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
	local seek = create_proportional_navigator(5, my_ship.max_acc)

	while true do
		t = sensor_contact(orders)

		if not t or energy() < 0.01*my_ship.energy.limit then
			explode()
		end

		local x, y = position()
		local vx, vy = velocity()
		local tx, ty = t:position()
		local tvx, tvy = t:velocity()

		seek(tx, ty, tvx, tvy)

		if distance(x,y,tx,ty)/distance(vx,vy,tvx,tvy) < 1/32 then explode() end
		yield()
	end
end

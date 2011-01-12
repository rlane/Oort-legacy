local min_dist = 2000
local max_dist = 3000

thrust(math.pi*5/4, 100)
sleep(64)

while true do
	if math.random(10) < 10 then
		clear_debug_lines()
		local t = sensor_contacts{ enemy=true }[1]
		local tx, ty = t:position()
		local x, y = position()
		local a = angle_between(x, y, tx, ty)
		local d = distance(x, y, tx, ty)
		if d < min_dist then
			thrust(a, -100*min_dist/d)
		elseif distance(x, y, tx, ty) > max_dist then
			thrust(a, 100*d/max_dist)
		else
			local vx, vy = position()
			a = angle_between(0, 0, vx, vy)
			thrust(a, -100)
		end
		yield()
	else
		thrust(math.random()*math.pi*2, 100)
		sleep(64)
	end
end

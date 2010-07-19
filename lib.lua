function printf(...)
	io.write(string.format(...))
end

function sleep(ticks)
	local i
	for i = 1,ticks do
		yield()
	end
end

function angle_between(x1, y1, x2, y2)
	local dx, dy, a
	dx = x2 - x1
	dy = y2 - y1
	a = math.atan2(dy, dx)
	if (a < 0) then
		a = 2*math.pi + a
	end
	return a
end

-- a*x^2 + b*x + c
function largest_root_of_quadratic_equation(a, b, c)
	z = math.sqrt(b^2 - 4*a*c)
	x1 = (b + z)/(2*a)
	x2 = (b - z)/(2*a)
	return math.max(x1, x2)
end

function lead(x1, y1, x2, y2, vx1, vy1, vx2, vy2, w, t_max)
	dx = x2 - x1
	dy = y2 - y1
	dvx = vx2 - vx1
	dvy = vy2 - vy1
	--printf("dvx=%0.2g dvy=%0.2g\n", dvx, dvy)
	a = (dvx^2 + dvy^2) - w^2
	b = 2 * (dx*dvx + dy*dvy)
	c = dx^2 + dy^2
	t = largest_root_of_quadratic_equation(a, b, c)
	--printf("t=%.03g\n", t)
	if t >= 0 and t <= t_max then
		x3 = x2 + dvx*t
		y3 = y2 + dvy*t
		--printf("x3=%0.2g y3=%0.2g\n", x3, y3)
		return angle_between(x1, y1, x3, y3)
	else
		return nil
	end
end

function distance(x1, y1, x2, y2)
	return math.sqrt((x2 - x1)^2 + (y2-y1)^2)
end

function pick_closest_enemy(x, y, enemy_team, max_dist)
	local contacts = sensor_contacts()
	local t = nil
	for k, c in pairs(contacts) do
		if c.team == enemy_team and distance(c.x, c.y, x, y) < max_dist then
			t = c
		end
	end

	if t then
		printf("target p=(%0.2g, %0.2g) v=(%0.2g, %0.2g)\n", t.x, t.y, t.vx, t.vy);
	else
		printf("no target\n")
	end

	return t
end

function enemy_team()
	local my_team = team()
	if my_team == "green" then
		return "blue"
	else
		return "green"
	end
end
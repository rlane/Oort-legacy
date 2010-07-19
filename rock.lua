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

local i = 0
local t = nil
local max_target_distance = 10

while true do
	local x, y = position()

	if i == 0 then
		local contacts = sensor_contacts()
		t = nil
		for k, c in pairs(contacts) do
			if c.team == 'green' and distance(c.x, c.y, x, y) < max_target_distance then
				t = c
			end
		end
	end

	i = i + 1
	if i == 4 then
		i = 0
	end

	local x, y = position()
	local vx, vy = velocity()

	if t then
		local a2 = lead(x, y, t.x, t.y, vx, vy, t.vx, t.vy, 10, 1)
		if a2 then
			fire(a2)
		end
	end

	yield()
end

target_debug = false

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

function pick_close_enemy(x, y, enemy_team, max_dist, prob)
	local contacts = sensor_contacts()
	local t = nil
	for k, c in pairs(contacts) do
		if c.team == enemy_team and distance(c.x, c.y, x, y) < max_dist and c.class ~= "missile" and (not t or (math.random() < prob)) then
			t = c
		end
	end

	if target_debug then
		if t then
			printf("target %s p=(%0.2g, %0.2g) v=(%0.2g, %0.2g)\n", t.class, t.x, t.y, t.vx, t.vy);
		else
			printf("no target\n")
		end
	end

	return t
end

function pick(t,fn)
	local t2 = select(t, fn)
	local ids = keys(t2)
	local n = table.maxn(ids)
	if not n then return end
	local k = ids[math.random(n)]
	return k, t[k]
end

function enemy_team()
	local my_team = team()
	if my_team == "green" then
		return "blue"
	else
		return "green"
	end
end

function R(a,b)
	d = b - a
	return a + math.random()*d
end

function normalize_angle(a)
	two_pi = math.pi * 2
	if a > two_pi then
		return a - two_pi
	elseif a < 0 then
		return a + two_pi
	else
		return a
	end
end

function sign(x)
	if x < 0 then
		return -1
	elseif x == 0 then
		return 0
	else
		return 1
	end
end

function select(t, fn)
	local r = {}
	for k, v in pairs(t) do
		if fn(k,v) then r[k] = v end
	end
	return r
end

function selecti(t, fn)
	local r = {}
	for k, v in ipairs(t) do
		if fn(v) then table.insert(r, v) end
	end
	return r
end

function keys_iter(t)
	local function itr(t,k)
		local k,v = next(t,k)
		return k
	end
	return itr, t, nil		
end

function keys(t)
	local r = {}
	for k in keys_iter(t) do
		table.insert(r, k)
	end
	return r
end

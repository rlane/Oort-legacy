target_debug = false
local my_team = team
local my_class = class
local my_ship = ships[my_class]
local two_pi = math.pi * 2

function printf(...)
	io.write(string.format(...))
end

function clamp(v,min,max)
	if v < min then return min
	elseif v > max then return max
	else return v
	end
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
		a = two_pi + a
	end
	return a
end

function angle_between_vec(a, b)
  return angle_between(a.x, a.y, b.x, b.y)
end

function angle_diff(a, b)
	local c = normalize_angle(b - a)
	if c > math.pi then
		c = c - two_pi
	end
	return c
end

-- a*x^2 + b*x + c
function smallest_positive_root_of_quadratic_equation(a, b, c)
	local z = math.sqrt(b^2 - 4*a*c)
	local x1 = (b + z)/(2*a)
	local x2 = (b - z)/(2*a)
	if x1 < 0 then return x2 end
	if x2 < 0 then return x1 end
	return math.min(x1, x2)
end

function lead(x1, y1, x2, y2, vx1, vy1, vx2, vy2, w, t_max)
	local dx = x2 - x1
	local dy = y2 - y1
	local dvx = vx2 - vx1
	local dvy = vy2 - vy1
	--printf("dvx=%0.2g dvy=%0.2g\n", dvx, dvy)
	local a = (dvx^2 + dvy^2) - w^2
	local b = 2 * (dx*dvx + dy*dvy)
	local c = dx^2 + dy^2
	local t = smallest_positive_root_of_quadratic_equation(a, b, c)
	--printf("t=%.03g\n", t)
	if t >= 0 and t <= t_max then
		local x3 = x2 + dvx*t
		local y3 = y2 + dvy*t
		--printf("x3=%0.2g y3=%0.2g\n", x3, y3)
		return angle_between(x1, y1, x3, y3)
	else
		return nil
	end
end

function distance(x1, y1, x2, y2)
	return math.sqrt((x2 - x1)^2 + (y2-y1)^2)
end

function pick_close_enemy(x, y, max_dist, prob)
	local contacts = sensor_contacts({})
	local t = nil
	for k, c in pairs(contacts) do
		local cx, cy = c:position()
		if c:team() ~= my_team and distance(cx, cy, x, y) < max_dist and c:class() ~= "torpedo" and (not t or (math.random() < prob)) then
			t = c
		end
	end

	if target_debug then
		if t then
			local tx, ty = t:position()
			local tvx, tvy = t:velocity()
			printf("target %s p=(%0.2g, %0.2g) v=(%0.2g, %0.2g)\n", t.class, tx, ty, tvx, tvy);
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
	if not n or n == 0 then return end
	local k = ids[math.random(n)]
	return k, t[k]
end

function min_by(t,fn)
	local best_value = nil
	local best_score = math.huge
	for k,v in pairs(t) do
		local score = fn(v)
		if fn(v) < best_score then
			best_value = v
			best_score = score
		end
	end
	return best_value
end

function R(a,b)
	d = b - a
	return a + math.random()*d
end

function normalize_angle(a)
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

function debug_square(x,y,l)
	debug_line(x-l, y-l, x+l, y-l)
	debug_line(x+l, y-l, x+l, y+l)
	debug_line(x+l, y+l, x-l, y+l)
	debug_line(x-l, y+l, x-l, y-l)
end

function debug_diamond(x,y,l)
	debug_line(x, y-l, x+l, y)
	debug_line(x+l, y, x, y+l)
	debug_line(x, y+l, x-l, y)
	debug_line(x-l, y, x, y-l)
end

function rotate(x,y,a)
	return x*math.cos(a) - y*math.sin(a), x*math.sin(a) + y*math.cos(a)
end

function accelerated_goto(p,v,a)
	local _a = a
	local _b = 2*v
	local _c = -p + v*v/(2*a)
	--printf("a=%0.3g b=%0.3g c=%0.3g\n", _a, _b, _c)
	local t = smallest_positive_root_of_quadratic_equation(_a,_b,_c)
	--printf("p=%0.3g v=%0.3g a=%0.3g t=%0.3g\n", p, v, a, t)
	if t > 0 then
		return 1
	else
		return -1
	end
end

function turn_to(angle)
	local h = heading()
	local w = angular_velocity()
	local wa = my_ship.max_angular_acc
	local diff = angle_diff(h,angle)
	local f = accelerated_goto(diff,-w,wa)
	thrust_angular(f*wa)
end

function turn_towards(tx,ty)
	local x, y = position()
	local a = angle_between(x, y, tx, ty)
	turn_to(a)
end

function turn_away(tx,ty)
	local x, y = position()
	local a = angle_between(tx, ty, x, y)
	turn_to(a)
end

function drive_towards(speed, tx, ty)
	local x, y = position()
	local vx, vy = velocity()
	local a = angle_between(x, y, tx, ty)
	local h = heading()

	local rvx, rvy = rotate(vx, vy, -h)
	thrust_lateral(-1*rvy)

	turn_to(a)

	local diff = angle_diff(a,h)
	if rvx > speed then
		thrust_main(speed-rvx)
	elseif math.abs(diff) < math.pi/4 then
		thrust_main(my_ship.max_main_acc)
	elseif math.abs(diff) > 3*math.pi/4 then
		thrust_main(-my_ship.max_main_acc)
	else
		thrust_main(0)
	end
end

function drive_towards_vec(speed, p)
  return drive_towards(speed, p.x, p.y)
end

function create_proportional_navigator(k, a, ev)
	local last_bearing = nil
	return function(tx, ty, tvx, tvy)
		local x, y = position()
		local bearing = angle_between(x, y, tx, ty)

		if last_bearing then
			local vx, vy = velocity()
			local h = heading()

			local bearing_rate = angle_diff(bearing, last_bearing)*32.0
			local dvx, dvy = vx-tvx, vy-tvy
			local rvx, rvy = rotate(dvx, dvy, -bearing)
			local n = -k*rvx*bearing_rate

			thrust_main(a, ev)
			thrust_lateral(n, ev)
			turn_to(bearing)
		end

		last_bearing = bearing
	end
end

function standard_missile_ai()
	local seek = create_proportional_navigator(5, my_ship.max_main_acc, 8e3)
	local i = 0

	while true do
		local t = sensor_contact(orders)

		if not t then
			local ts = sensor_contacts{ enemy = true }
			local x, y = position()
			local min_fn = function(t)
				local tx, ty = t:position()
				return distance(x, y, tx, ty)
			end
			t = min_by(ts, min_fn)
		end

		if not t or energy() < 0.01*my_ship.energy.limit then
			explode()
		end

		local x, y = position()
		local vx, vy = velocity()
		local tx, ty = t:position()
		local tvx, tvy = t:velocity()

		clear_debug_lines()
		debug_diamond(tx, ty, 16*my_ship.radius)

		if i < 16 then
			i = i + 1
			turn_towards(tx, ty)
			thrust_main(i*my_ship.max_main_acc/16)
		else
			seek(tx, ty, tvx, tvy)
		end

		if distance(x,y,tx,ty)/distance(vx,vy,tvx,tvy) < 1/32 then explode() end
		yield()
	end
end

function chance(n)
	return math.random(1,n*32) == 1
end

function fire_at_contact(gun_name, t)
	if not check_gun_ready(gun_name) then
		return
	end

	local gun = my_ship.guns[gun_name]
	local x, y = position()
	local tx, ty = t:position()
	if gun.type == "bullet" then
		local vx, vy = velocity()
		local tvx, tvy = t:velocity()
		local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, gun.velocity, gun.ttl)
		if a then
			fire(gun_name, a)
		end
	elseif gun.type == "beam" then
		if distance(x, y, tx, ty) < gun.length then
			local a = angle_between(x, y, tx, ty)
			fire(gun_name, a)
		end
	end
end

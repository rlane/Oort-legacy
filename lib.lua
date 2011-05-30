local my_ship = ships[class]
local two_pi = math.pi * 2

function printf(...)
	io.write(string.format(...))
end

--- Limit a value to a range
function clamp(v,min,max)
	if v < min then return min
	elseif v > max then return max
	else return v
	end
end

function angle_between(x1, y1, x2, y2)
	return angle_between_vec(vec(x1,y1), vec(x2, y2))
end

--- Return the angle between two vectors
function angle_between_vec(p1, p2)
	return (p2-p1):angle()
end

--- Return the difference between two angles
-- Result may be negative.
function angle_diff(a, b)
	local c = normalize_angle(b - a)
	if c > math.pi then
		c = c - two_pi
	end
	return c
end

--- Solve the quadratic equation
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
	return lead_vec(vec(x1,y1), vec(x2,y2), vec(vx1,vy1), vec(vx2,vy2), w, t_max)
end

--- Return the angle to shoot a constant-velocity projectile to hit a moving target
function lead_vec(p1, p2, v1, v2, w, t_max)
	local dp = p2 - p1
	local dv = v2 - v1
	local a = (dv.x^2 + dv.y^2) - w^2
	local b = 2 * (dp.x*dv.x + dp.y*dv.y)
	local c = dp.x^2 + dp.y^2
	local t = smallest_positive_root_of_quadratic_equation(a, b, c)
	--printf("t=%.03g\n", t)
	if t >= 0 and t <= t_max then
		local p3 = p2 + dv*t
		--printf("x3=%0.2g y3=%0.2g\n", x3, y3)
		return angle_between_vec(p1, p3)
	else
		return nil
	end
end

function distance(x1, y1, x2, y2)
	return math.sqrt((x2 - x1)^2 + (y2-y1)^2)
end

--- Choose a random integer-keyed member from a table
function pick(t,fn)
	local t2 = select(t, fn)
	local ids = keys(t2)
	local n = table.maxn(ids)
	if not n or n == 0 then return end
	local k = ids[math.random(n)]
	return k, t[k]
end

--- Return the value from the given table where fn returns the lowest score
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

--- Return a floating point value in the given range
function R(a,b)
	local d = b - a
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

function turn_towards(x, y)
	turn_towards_vec(vec(x,y))
end

function turn_towards_vec(p)
	turn_to((p - position_vec()):angle())
end

function drive_towards(speed, tx, ty)
	drive_towards_vec(speed, vec(tx,ty))
end

function drive_towards_vec(speed, tp)
	local p = position_vec()
	local v = velocity_vec()
	local a = (tp - p):angle()
	local h = heading()

	local rv = v:rotate(-h)
	thrust_lateral(-1*rv.y)

	turn_to(a)

	local diff = angle_diff(a,h)
	if rv.x > speed then
		thrust_main(speed-rv.x)
	elseif math.abs(diff) < math.pi/4 then
		thrust_main(my_ship.max_main_acc)
	elseif math.abs(diff) > 3*math.pi/4 then
		thrust_main(-my_ship.max_main_acc)
	else
		thrust_main(0)
	end
end

function create_proportional_navigator(k, a, ev)
	local last_bearing = nil
	return function(tp, tv)
		local p = position_vec()
		local bearing = angle_between_vec(p, tp)

		if last_bearing then
			local v = velocity_vec()
			local h = heading()

			local bearing_rate = angle_diff(bearing, last_bearing)/tick_length
			local dv = v-tv
			local rv = dv:rotate(-bearing)
			local n = -k*rv.x*bearing_rate

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
			local p = position_vec()
			local min_fn = function(t)
				return p:distance(t:position_vec())
			end
			t = min_by(ts, min_fn)
		end

		if not t or energy() < 0.01*my_ship.energy.limit then
			explode()
		end

		local p = position_vec()
		local v = velocity_vec()
		local tp = t:position_vec()
		local tv = t:velocity_vec()

		clear_debug_lines()
		debug_diamond(tp.x, tp.y, 16*my_ship.radius)

		if i < 16 then
			i = i + 1
			turn_towards_vec(tp)
			thrust_main(i*my_ship.max_main_acc/16)
		else
			seek(tp, tv)
		end

		if p:distance(tp)/v:distance(tv) < tick_length then explode() end
		yield()
	end
end

function chance(n)
	return math.random(1,n/tick_length) == 1
end

function fire_at_contact(gun_name, t)
	if not check_gun_ready(gun_name) then
		return
	end

	local gun = my_ship.guns[gun_name]
	local p = position_vec()
	local tp = t:position_vec()
	if gun.type == "bullet" then
		local v = velocity_vec()
		local tv = t:velocity_vec()
		local a = lead_vec(p, tp, v, tv, gun.velocity, gun.ttl)
		if a then
			fire(gun_name, a)
		end
	elseif gun.type == "beam" then
		if p:distance(tp) < gun.length then
			local a = angle_between_vec(p, tp)
			fire(gun_name, a)
		end
	end
end

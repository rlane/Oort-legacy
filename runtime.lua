lib()

-- constants
local my_class = class
local my_ship = ships[my_class]
local ticks_per_second = 32
local tick_length = 1.0/ticks_per_second
local default_exhaust_velocity = 10e3
local debug_preemption = false

local last_fire_ticks = {}
local last_spawn_ticks = {}
local _energy = my_ship.energy.initial
local _reaction_mass = my_ship.reaction_mass
local energy_tick_rate = my_ship.energy.rate * tick_length
local ticks = 0

-- engine consumption per tick
local engine_main_power_cost = 0
local engine_main_mass_cost = 0
local engine_lateral_power_cost = 0
local engine_lateral_mass_cost = 0
local engine_angular_power_cost = 0
local engine_angular_mass_cost = 0

function energy()
	return _energy
end

function reaction_mass()
	return _reaction_mass
end

function thrust_main(acc,exhaust_velocity)
	exhaust_velocity = exhaust_velocity or default_exhaust_velocity
	acc = clamp(acc,-my_ship.max_main_acc,my_ship.max_main_acc);
	local dv = math.abs(acc)*tick_length
	engine_main_mass_cost = my_ship.mass*dv/exhaust_velocity
	engine_main_power_cost = 0.5*engine_main_mass_cost*exhaust_velocity^2
	sys_thrust_main(acc)
end

function thrust_lateral(acc,exhaust_velocity)
	exhaust_velocity = exhaust_velocity or default_exhaust_velocity
	acc = clamp(acc,-my_ship.max_lateral_acc,my_ship.max_lateral_acc);
	local dv = math.abs(acc)*tick_length
	engine_lateral_mass_cost = my_ship.mass*dv/exhaust_velocity
	engine_lateral_power_cost = 0.5*engine_lateral_mass_cost*exhaust_velocity^2
	sys_thrust_lateral(acc)
end

function thrust_angular(acc,exhaust_velocity)
	exhaust_velocity = exhaust_velocity or default_exhaust_velocity
	acc = clamp(acc,-my_ship.max_angular_acc,my_ship.max_angular_acc);
	local dv = math.abs(acc)*tick_length
	engine_angular_mass_cost = my_ship.mass*dv/exhaust_velocity
	engine_angular_power_cost = 0.5*engine_angular_mass_cost*exhaust_velocity^2
	sys_thrust_angular(acc)
end

function fire(name, a)
	local x,y,v,vx,vy,m,ttl
	local gun = my_ship.guns[name]
	
	if not gun then
		error(string.format("gun %s does not exist", name))
	end

	if gun.type == "bullet" then
		if _energy < gun.cost then
			return
		end

		if gun.refuel and _reaction_mass < gun.mass then
			return
		end

		if math.abs(angle_diff(normalize_angle(sys_heading() + gun.angle), a)) > gun.coverage/2 then
			return
		end

		if check_gun_ready(name) then
			last_fire_ticks[name] = ticks
		else
			return
		end

		_energy = _energy - gun.cost

		if gun.refuel then
			_reaction_mass = _reaction_mass - gun.mass
		end

		x, y = sys_position()
		v = gun.velocity
		vx, vy = sys_velocity()
		a = a + gun.spread*(math.random() - 0.5)
		vx = vx + math.cos(a)*v
		vy = vy + math.sin(a)*v
		sys_create_bullet(x, y, vx, vy, gun.mass, gun.radius, gun.ttl, gun.graphics)
	elseif gun.type == "beam" then
		local cost = gun.cost * tick_length

		if _energy < cost then
			return
		end

		if math.abs(angle_diff(normalize_angle(sys_heading() + gun.angle), a)) > gun.coverage/2 then
			return
		end

		_energy = _energy - cost

		x, y = sys_position()
		sys_create_beam(x, y, a, gun.length, gun.width, gun.damage, gun.graphics)
	end
end

function check_gun_ready(name)
	local gun = my_ship.guns[name]
	if gun.type == "bullet" then
		local last_fire_tick = last_fire_ticks[name]
		if last_fire_tick then
			return last_fire_tick + gun.reload_time*ticks_per_second < ticks
		else
			return true
		end
	else
		return true
	end
end

function sensor_contacts(query)
	return sys_sensor_contacts(query)
end

function sensor_contact(id)
	return sys_sensor_contact(id)
end

function send(msg)
	local cost = string.len(msg)

	if _energy < cost then
		return
	else
		_energy = _energy - cost
	end

	sys_send(msg)
end

function recv()
	return sys_recv()
end

function spawn(class, orders)
	local ship = ships[class]

	if not ship then
		error(string.format("class %s does not exist", class))
	end

	local cooldown_time = my_ship.spawnable[class]
	if not cooldown_time then
		error(string.format("class %s is not spawnable by %s", class, my_class))
	end

	if _energy < ship.cost then
		return
	end

	local last_spawn_tick = last_spawn_ticks[class]
	if last_spawn_tick and last_spawn_tick + cooldown_time*ticks_per_second > ticks then
		return
	else
		last_spawn_ticks[class] = ticks
	end

	_energy = _energy - ship.cost

	sys_spawn(class, orders)
end

function explode()
	local x, y = sys_position()
	--local e = _energy + (my_ship.warhead or 0)
	local e = my_ship.warhead or 0
	local ray_energy = 1e3
	local len = 200
	local max_rays = 64
	local n = math.floor(e/ray_energy)
	if n > max_rays then
		ray_energy = ray_energy*n/max_rays
		n = max_rays
	end
	local v = len/tick_length
	local m = 2*ray_energy/(v*v)
	local r = 1
	local da = 2*math.pi/n
	local ttl = tick_length

	local a = 0
	local i
	for i = 1,n do
		local vx = math.cos(a)*v
		local vy = math.sin(a)*v
		sys_create_bullet(x,y,vx,vy,m,r,ttl,bullets.explosion)
		a = a + da
	end

	sys_die()
end

function yield()
	coroutine.yield()
end

sandbox_api = {
	thrust_main = thrust_main,
	thrust_lateral = thrust_lateral,
	thrust_angular = thrust_angular,
	position = sys_position,
	heading = sys_heading,
	velocity = sys_velocity,
	angular_velocity = sys_angular_velocity,
	reaction_mass = reaction_mass,
	energy = energy,
	fire = fire,
	check_gun_ready = check_gun_ready,
	yield = yield,
	sensor_contacts = sensor_contacts,
	sensor_contact = sensor_contact,
	send = send,
	recv = recv,
	spawn = spawn,
	explode = explode,
	debug_line = sys_debug_line,
	clear_debug_lines = sys_clear_debug_lines,
}

function copy_table(t, t2)
	for k,v in pairs(t) do
		t2[k] = v
	end
	return t2
end

-- TODO limit files that can be read
function safe_dofile(name)
	local f,e = loadfile(name)
		if not f then error(e, 2) end
		setfenv(f, getfenv(2))
	return f()
end

function debug_count_hook()
	if debug_preemption then
		print("preempted", my_ship_name, ship_id)
		print(debug.traceback())
	end
end

function tick_hook()
	ctl_tick_hook();

	local power_draw = engine_main_power_cost + engine_lateral_power_cost + engine_angular_power_cost
	if _energy < power_draw then
		--print(my_class, " engine cost too high:", engine_power_cost, " > ", _energy)
		thrust_main(0)
		thrust_lateral(0)
		thrust_angular(0)
	else
		_energy = _energy - power_draw
	end

	local reaction_mass_draw = engine_main_mass_cost + engine_lateral_mass_cost + engine_angular_mass_cost
	if _reaction_mass < reaction_mass_draw then
		--print(my_class, " engine cost too high:", engine_power_cost, " > ", _energy)
		thrust_main(0)
		thrust_lateral(0)
		thrust_angular(0)
	else
		_reaction_mass = _reaction_mass - reaction_mass_draw
	end

	_energy = _energy + energy_tick_rate
	if _energy > my_ship.energy.limit then
		_energy = my_ship.energy.limit
	end
	ticks = ticks + 1
end

function refuel_hit(m)
	_reaction_mass = math.min(_reaction_mass+m, my_ship.reaction_mass)
end

function sandbox(f)
	local env = {
		error = error,
		assert = assert,
		ipairs = ipairs,
		next = next,
		pairs = pairs,
		print = print,
		select = select,
		tonumber = tonumber,
		tostring = tostring,
		type = type,
		unpack = unpack,

		-- dofile = safe_dofile,
		io = { write = io.write },

		id = id,
		orders = orders,
		class = my_class,
		team = team,
		scenario_radius = scenario_radius,
		ships = copy_table(ships, {})
	}

	env._G = env
	env.math = copy_table(math, {})
	env.math.random = sys_random
	env.math.randomseed = nil
	env.table = copy_table(table, {})
	env.string = copy_table(string, {})
	env.string.dump = nil

	copy_table(sandbox_api, env)

	setfenv(lib, env)
	lib()

	strict(env._G)

	setfenv(f, env)
	return f
end

function control_begin()
	thrust_main(0)
	thrust_lateral(0)
	thrust_angular(0)
end

function control_end()
	thrust_main(0)
	thrust_lateral(0)
	thrust_angular(0)
end

local ctl_fwd = false
local ctl_back = false
local ctl_left = false
local ctl_right = false
local ctl_turn_left = false
local ctl_turn_right = false
local ctl_fire = false
function control(key,pressed)
	if key == 'w' then ctl_fwd = pressed end
	if key == 's' then ctl_back = pressed end
	if key == 'a' then ctl_left = pressed end
	if key == 'd' then ctl_right = pressed end
	if key == 'j' then ctl_turn_left = pressed end
	if key == 'l' then ctl_turn_right = pressed end
	if key == 'i' then ctl_fire = pressed end

	local main = 0
	local lateral = 0
	local angular = 0

	if ctl_fwd then main = main + 1 end
	if ctl_back then main = main - 1 end
	if ctl_left then lateral = lateral - 1 end
	if ctl_right then lateral = lateral + 1 end
	if ctl_turn_left then angular = angular - 1 end
	if ctl_turn_right then angular = angular + 1 end

	thrust_main(my_ship.max_main_acc*main)
	thrust_lateral(my_ship.max_lateral_acc*lateral)
	thrust_angular(1*angular)
end

function ctl_tick_hook()
	if ctl_fire then
		fire("main", sys_heading())
	end
end

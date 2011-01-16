-- constants
local my_class = class
local my_ship = ships[my_class]
local ticks_per_second = 32
local tick_length = 1.0/ticks_per_second
local exhaust_velocity = 10e3
local debug_preemption = false

local last_fire_ticks = {}
local last_spawn_ticks = {}
local _energy = my_ship.energy.initial
local energy_tick_rate = my_ship.energy.rate * tick_length
local ticks = 0

-- engine consumption per tick
local engine_power_cost = 0
local engine_mass_cost = 0

function energy()
	return _energy
end

function thrust(a,acc)
	if acc < 0 then
		acc = -acc
		a = a + math.pi
	end

	if acc > my_ship.max_acc then
		acc = my_ship.max_acc
	end

	local dv = acc*tick_length
	engine_mass_cost = my_ship.mass*dv/exhaust_velocity
	engine_power_cost = 0.5*engine_mass_cost*exhaust_velocity^2

	sys_thrust(a,acc)
end

function fire(name, a)
	local x,y,v,vx,vy,m,ttl
	local gun = my_ship.guns[name]
	
	if not gun then
		error(string.format("gun %s does not exist", name))
	end

	local last_fire_tick = last_fire_ticks[name]

	if _energy < gun.cost then
		return
	end

	if last_fire_tick and last_fire_tick + gun.reload_time*ticks_per_second > ticks then
		return
	else
		last_fire_ticks[name] = ticks
	end

	_energy = _energy - gun.cost

	x, y = sys_position()
	v = gun.bullet_velocity
	vx, vy = sys_velocity()
	a = a + gun.spread*(math.random() - 0.5)
	vx = vx + math.cos(a)*v
	vy = vy + math.sin(a)*v
	m = gun.bullet_mass
	ttl = gun.bullet_ttl
	sys_create_bullet(x,y,vx,vy,m,ttl,gun.bullet_type)
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
	local da = 2*math.pi/n
	local ttl = tick_length

	local a = 0
	local i
	for i = 1,n do
		local vx = math.cos(a)*v
		local vy = math.sin(a)*v
		sys_create_bullet(x,y,vx,vy,m,ttl,bullets.explosion)
		a = a + da
	end

	sys_die()
end

function yield()
	coroutine.yield()
end

sandbox_api = {
	thrust = thrust,
	position = sys_position,
	velocity = sys_velocity,
	reaction_mass = reaction_mass,
	energy = energy,
	fire = fire,
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
	if _energy < engine_power_cost then
		--print(my_class, " engine cost too high:", engine_power_cost, " > ", _energy)
		thrust(0,0)
	end
	_energy = _energy - engine_power_cost

	_energy = _energy + energy_tick_rate
	if _energy > my_ship.energy.limit then
		_energy = my_ship.energy.limit
	end
	ticks = ticks + 1
end

function sandbox(f)
	local env = {
		_G = {},
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

		orders = orders,
		class = my_class,
		team = team,
		ships = copy_table(ships, {})
	}

	env.math = copy_table(math, {})
	env.math.random = sys_random
	env.math.randomseed = nil
	env.table = copy_table(table, {})
	env.string = copy_table(string, {})
	env.string.dump = nil

	copy_table(sandbox_api, env)

	setfenv(lib, env)
	lib()

	setfenv(f, env)
	return f
end

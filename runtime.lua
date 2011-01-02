local my_class = class
local my_ship = ships[my_class]
local last_fire_ticks = {}
local _energy = my_ship.energy.initial
local energy_tick_rate = my_ship.energy.rate / 32.0
local debug_preemption = false
local ticks = 0

function energy()
	return _energy
end

function thrust(a,acc)
	if acc < 0 then
		return
	end

	if acc > my_ship.max_acc then
		acc = my_ship.max_acc
	end

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
	else
		_energy = _energy - gun.cost
	end

	if last_fire_tick and last_fire_tick + gun.reload_time*32 > ticks then
		return
	else
		last_fire_ticks[name] = ticks
	end

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

function check_spawnable(class)
	for i,v in ipairs(my_ship.spawnable) do
		if v == class then return true end
	end
	return false
end

function spawn(class, orders)
	local ship = ships[class]

	if not ship then
		error(string.format("class %s does not exist", class))
	end

	if not check_spawnable(class) then
		error(string.format("class %s is not spawnable by %s", class, my_class))
	end

	if _energy < ship.cost then
		return
	else
		_energy = _energy - ship.cost
	end

	sys_spawn(class, orders)
end

function explode()
	local x, y = sys_position()
	local vx, vy = sys_velocity()
	local i
	local n = 128
	local exp = my_ship.explosion

	for i = 1,exp.count do
		local a, v, m, ttl, vx2, vy2
		a = math.random()*math.pi*2
		v = math.random()*exp.velocity
		vx2 = vx + math.cos(a)*v
		vy2 = vy + math.sin(a)*v
		m = exp.mass
		ttl = exp.ttl
		sys_create_bullet(x,y,vx2,vy2,m,ttl,bullets.plasma)
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

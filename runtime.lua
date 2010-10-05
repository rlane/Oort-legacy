dofile("ships.lua")

local my_class = ships[sys_class()]
local last_fire_times = {}
local _energy = my_class.energy.initial
local energy_tick_rate = my_class.energy.rate / 32.0

function energy()
	return _energy
end

function thrust(a,acc)
	if acc < 0 then
		return
	end

	if acc > my_class.max_acc then
		acc = my_class.max_acc
	end

	sys_thrust(a,acc)
end

function fire(name, a)
	local x,y,v,vx,vy,m,ttl
	local gun = my_class.guns[name]
	
	if not gun then
		error(string.format("gun %s does not exist", name))
	end

	local last_fire_time = last_fire_times[name]

	if _energy < gun.cost then
		return
	else
		_energy = _energy - gun.cost
	end

	if last_fire_time and last_fire_time + gun.reload_time > sys_time() then
		return
	else
		last_fire_times[name] = sys_time()
	end

	x, y = sys_position()
	v = gun.bullet_velocity
	vx, vy = sys_velocity()
	vx = vx + math.cos(a)*v
	vy = vy + math.sin(a)*v
	m = gun.bullet_mass
	ttl = gun.bullet_ttl
	sys_create_bullet(x,y,vx,vy,m,ttl)
end

function sensor_contacts()
	return sys_sensor_contacts()
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

function spawn(class_name, filename, orders)
	class = ships[class_name]

	if not class then
		error(string.format("class %s does not exist", class_name))
		return
	end

	if _energy < class.cost then
		return
	else
		_energy = _energy - class.cost
	end

	sys_spawn(class_name, filename, orders)
end

function explode()
	local x, y = sys_position()
	local vx, vy = sys_velocity()
	local i
	local n = 128
	local exp = my_class.explosion

	for i = 1,exp.count do
		local a, v, m, ttl, vx2, vy2
		a = math.random()*math.pi*2
		v = math.random()*exp.velocity
		vx2 = vx + math.cos(a)*v
		vy2 = vy + math.sin(a)*v
		m = exp.mass
		ttl = exp.ttl
		sys_create_bullet(x,y,vx2,vy2,m,ttl)
	end

	sys_die()
end

sandbox_api = {
	thrust = thrust,
	position = sys_position,
	velocity = sys_velocity,
	energy = energy,
	class = sys_class,
	fire = fire,
	yield = sys_yield,
	team = sys_team,
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
	print("preempted", sys_class(), ship_id)
	print(debug.traceback())
end

function tick_hook()
	_energy = _energy + energy_tick_rate
	if _energy > my_class.energy.limit then
		_energy = my_class.energy.limit
	end
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

		dofile = safe_dofile,
		io = { write = io.write },

		orders = orders,
		ships = copy_table(ships, {})
	}

	env.math = copy_table(math, {})
	env.math.random = sys_random
	env.math.randomseed = nil
	env.table = copy_table(table, {})
	env.string = copy_table(string, {})
	env.string.dump = nil

	copy_table(sandbox_api, env)

	setfenv(f, env)

	return f
end

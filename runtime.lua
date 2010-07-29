dofile("ships.lua")

local my_class = ships[sys_class()]
local last_fire_times = {}

function thrust(a,f)
	sys_thrust(a,f)
end

function position()
	return sys_position()
end

function velocity()
	return sys_velocity()
end

function fire(name, a)
	local x,y,v,vx,vy,m,ttl
	local gun = my_class.guns[name]
	
	if not gun then
		error(string.format("gun %s does not exist", name))
	end

	local last_fire_time = last_fire_times[name]

	if last_fire_time and last_fire_time + gun.reload_time > sys_time() then
		return
	else
		last_fire_times[name] = sys_time()
	end

	x, y = position()
	v = gun.bullet_velocity
	vx, vy = velocity()
	vx = vx + math.cos(a)*v
	vy = vy + math.sin(a)*v
	m = gun.bullet_mass
	ttl = gun.bullet_ttl
	sys_create_bullet(x,y,vx,vy,m,ttl)
end

function yield()
	sys_yield()
end

function team()
	return sys_team()
end

function sensor_contacts()
	return sys_sensor_contacts()
end

function class()
	return sys_class();
end

function send(msg)
	sys_send(msg)
end

function recv()
	return sys_recv()
end

function spawn(class, filename, orders)
	sys_spawn(class, filename, orders)
end

function explode()
	local x, y = position()
	local vx, vy = velocity()
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
	position = position,
	velocity = velocity,
	fire = fire,
	yield = yield,
	team = team,
	sensor_contacts = sensor_contacts,
	send = send,
	recv = recv,
	spawn = spawn,
	explode = explode,
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

lib()
vector()

function time()
	error("NYI")
end

function position_vec()
	local x, y = sys_position()
	return vec(x, y)
end

function velocity_vec()
	local vx, vy = sys_velocity()
	return vec(vx, vy)
end

function thrust_main(acc,exhaust_velocity)
	sys_thrust_main(acc)
end

function thrust_lateral(acc,exhaust_velocity)
	sys_thrust_lateral(acc)
end

function thrust_angular(acc,exhaust_velocity)
	sys_thrust_angular(acc)
end

function fire_gun(idx, a)
	sys_fire_gun(idx, a)	
end

function check_gun_ready(name)
	error("NYI")
end

function sensor_contacts(query)
	error("NYI")
end

function sensor_contact(id)
	error("NYI")
end

function send(msg)
	error("NYI")
end

function recv()
	error("NYI")
end

function spawn(class, orders)
	error("NYI")
end

function explode()
	error("NYI")
end

function yield()
	coroutine.yield()
end

logbuf_head = 0
logbuf_max = 128
logbuf_lines = {}

function log(...)
	local line = string.format(...)
	logbuf_head = logbuf_head + 1
	local del = logbuf_head - logbuf_max
	if logbuf_lines[del] then logbuf_lines[del] = nil end
	logbuf_lines[logbuf_head] = line
end

sandbox_api = {
	thrust_main = thrust_main,
	thrust_lateral = thrust_lateral,
	thrust_angular = thrust_angular,
	position = sys_position,
	position_vec = position_vec,
	heading = sys_heading,
	velocity = sys_velocity,
	velocity_vec = velocity_vec,
	angular_velocity = sys_angular_velocity,
	reaction_mass = reaction_mass,
	energy = energy,
	fire_gun = fire_gun,
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
	time = time,
	log = log,
}

function copy_table(t, t2)
	for k,v in pairs(t) do
		t2[k] = v
	end
	return t2
end

function debug_count_hook()
	if debug_preemption then
		print("preempted", team, class, hex_id)
		print(debug.traceback())
	end
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

		vec = vec,
	}

	env._G = env
	env.math = copy_table(math, {})
	env.math.random = sys_random
	env.math.randomseed = nil
	env.table = copy_table(table, {})
	env.string = copy_table(string, {})
	env.string.dump = nil

	copy_table(sandbox_api, env)

	if setfenv then
		setfenv(lib, env)
	else
		debug.setupvalue(lib, 1, env)
	end
	lib()

	strict(env._G)

	if setfenv then
		setfenv(f, env)
	else
		debug.setupvalue(f, 1, env)
	end
	return f
end

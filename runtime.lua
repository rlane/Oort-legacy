function thrust(a,f)
	sys_thrust(a,f)
end

function position()
	return sys_position()
end

function velocity()
	return sys_velocity()
end

function fire(a)
	sys_fire(a)
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

sandbox_api = {
	thrust = thrust,
	position = position,
	velocity = velocity,
	fire = fire,
	yield = yield,
	team = team,
	sensor_contacts = sensor_contacts,
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
	}

	env.math = copy_table(math, {})
	copy_table(sandbox_api, env)

	setfenv(f, env)

	return f
end

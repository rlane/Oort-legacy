local mt = {}
local methods = {}

local sqrt, cos, sin = math.sqrt, math.cos, math.sin

vector_metatable = mt -- XXX optimize

function vec(x, y)
	local u = { x, y }
	setmetatable(u, mt)
	return u
end

function mt:__index(key)
	if key == "x" then return self[1]
	elseif key == "y" then return self[2]
	elseif methods[key] then return methods[key]
	else error("attempt to access nonexistent field in vector")
	end
end

function mt:__tostring()
	return "(" .. self[1] .. ", " .. self[2] .. ")"
end

function mt:__add(u)
	return vec(self[1] + u[1], self[2] + u[2])
end

function mt:__sub(u)
	return vec(self[1] - u[1], self[2] - u[2])
end

function mt:__mul(c)
	return vec(self[1] * c, self[2] * c)
end

function mt:__div(c)
	return vec(self[1] / c, self[2] / c)
end

function mt:__unm()
	return vec(-self[1], -self[2])
end

function methods:length()
	local x, y = self[1], self[2]
	return sqrt(x^2 + y^2)
end

function methods:dot(u)
	return self[1]*u[1] + self[2]*u[2]
end

function methods:rotate(a)
	return vec(self[1]*cos(a) - self[2]*sin(a),
	           self[1]*sin(a) + self[2]*cos(a))
end

function methods:distance(u)
	local dx = u[1] - self[1]
	local dy = u[2] - self[2]
	return sqrt(dx^2 + dy^2)
end

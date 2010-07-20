function R(a,b)
	d = b - a
	return a + math.random()*d
end

for i = 1,16 do
	ship("fighter", "orbit.lua", "blue", R(5,6), R(1,2))
	ship("fighter", "orbit.lua", "green", R(-6,-5), R(-2,-1))
end

ship("mothership", "rock.lua", "blue", 0, 6)
ship("mothership", "rock.lua", "green", 0, -6)

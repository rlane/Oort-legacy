function R(a,b)
	d = b - a
	return a + math.random()*d
end

team("blue",  0x0000FF00)
team("green", 0x00FF0000)

for i = 1,32 do
	ship("fighter", "examples/orbit.lua", "blue", R(15,20), R(10,20))
	ship("fighter", "examples/orbit.lua", "green", R(-15,-20), R(-20,-10))
end

function mothership_circle(X, Y, n, r, filename, team)
	for i = 1,n do
		local a = i*(math.pi*2)/n
		local x = X + math.cos(a)*r
		local y = Y + math.sin(a)*r
		ship("mothership", filename, team, x, y)
	end
end

mothership_circle(0, -40, 3, 5, "examples/rock.lua", "green")
mothership_circle(0, 40, 3, 5, "examples/rock.lua", "blue")

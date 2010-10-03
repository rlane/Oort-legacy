if N ~= 2 then
	error("this scenario only supports 2 teams")
end

function R(a,b)
	d = b - a
	return a + math.random()*d
end

team("blue",  0x0000FF00)
team("green", 0x00FF0000)

blue_code = AI[0]
green_code = AI[1]

for i = 1,32 do
	ship("fighter", blue_code, "blue", R(20,15), R(20,10))
	ship("fighter", green_code, "green", R(-20,-15), R(-10,-20))
end

function mothership_circle(X, Y, n, r, filename, team)
	for i = 1,n do
		local a = i*(math.pi*2)/n
		local x = X + math.cos(a)*r
		local y = Y + math.sin(a)*r
		ship("mothership", filename, team, x, y)
	end
end

mothership_circle(-40, 0, 3, 5, green_code, "green")
mothership_circle(40, 0, 3, 5, blue_code, "blue")

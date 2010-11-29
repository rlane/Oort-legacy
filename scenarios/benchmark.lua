-- name: benchmark
-- author: Rich Lane
-- version: 1.0
-- description: 
-- min_teams: 2
-- max_teams: 2

if N ~= 0 then
	error("this scenario takes no arguments")
end

function R(a,b)
	d = b - a
	return a + math.random()*d
end

blue_code = data_dir .. "/examples/switch.lua"
green_code = data_dir .. "/examples/switch.lua"

team("blue",  blue_code, 0x0000FF00)
team("green", green_code, 0x00FF0000)

for i = 1,512 do
	ship("fighter", "blue", R(20,15), R(20,10))
	ship("fighter", "green", R(-20,-15), R(-10,-20))
end

function mothership_circle(X, Y, n, r, team)
	for i = 1,n do
		local a = i*(math.pi*2)/n
		local x = X + math.cos(a)*r
		local y = Y + math.sin(a)*r
		ship("mothership", team, x, y)
	end
end

mothership_circle(-40, 0, 12, 5, "green")
mothership_circle(40, 0, 12, 5, "blue")

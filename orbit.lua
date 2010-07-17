pi = 3.1415927

function sleep(ticks)
	local i
	for i = 1,ticks do
		yield()
	end
end

function angle_between(x1, y1, x2, y2)
	local dx, dy, a
	dx = x2 - x1
	dy = y2 - y1
	a = math.atan2(dy, dx)
	if (a < 0) then
		a = 2*pi + a
	end
	return a
end

function distance(x1, y1, x2, y2)
	return math.sqrt((x2 - x1)^2 + (y2-y1)^2)
end


thrust(0, 10)
sleep(32)
thrust(0, -10)
sleep(32)
thrust(0, 0)

sleep(32)

thrust(pi/2, 1)
sleep(32)

while true do
	x, y = position()
	a = angle_between(x, y, 0, 0)
	--io.write(string.format("x=%g y=%g a=%g\n", x, y, a))
	thrust(a, 20.0/(distance(x, y, 0, 0)^2))
	fire(a)
	yield()
end

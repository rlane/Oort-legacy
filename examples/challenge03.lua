thrust(1.57, 10)
for i = 1,10 do
	yield()
end

while true do
	yield()
	x, y = position()
	thrust(angle_between(x,y,0,0), 1)
end

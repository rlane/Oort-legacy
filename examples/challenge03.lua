local my_ship = ships[class]
thrust(1.57, my_ship.max_acc)
for i = 1,32 do
	yield()
end

while true do
	yield()
	x, y = position()
	thrust(angle_between(x,y,0,0), my_ship.max_acc/3)
end

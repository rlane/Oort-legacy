local min_dist = 2000
local max_dist = 3000
local my_ship = ships[class]

for i = 1,64 do
	turn_to(math.pi*5/4)
	yield()
end

thrust_main(my_ship.max_main_acc/10)
sleep(64)

while true do
	local t = sensor_contacts{ enemy=true, class="fighter" }[1]
	local tx, ty = t:position()
	local x, y = position()
	local d = distance(x, y, tx, ty)
	if d < min_dist then
		turn_away(tx,ty)
	elseif distance(x, y, tx, ty) > max_dist then
		turn_towards(tx,ty)
	else
		turn_towards(0,0)
	end
	yield()
end

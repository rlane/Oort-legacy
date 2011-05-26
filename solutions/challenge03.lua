local my_ship = ships[class]
local target = nil

while true do
	if not target then
		target = sensor_contacts{ enemy=true, limit=1 }[1]
	end

	if target then
		local x, y = position()
		local vx, vy = velocity()
		local tx, ty = target:position()
		local tvx, tvy = target:velocity()
		local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.velocity, my_ship.guns.main.ttl)
		drive_towards(300, tx, ty)
		if (a) then fire("main", a) end
	end

	yield()
end

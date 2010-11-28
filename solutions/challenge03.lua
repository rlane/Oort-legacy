dofile(data_dir .. "/examples/lib.lua")

function target_selector(k,c)
	return c:team() ~= team()
end

my_ship = ships[class()]

while true do
	local contacts = sensor_contacts()
	local tid, t = pick(contacts, target_selector)
	local x, y = position()
	local vx, vy = velocity()
	local tx, ty = t:position()
	local tvx, tvy = t:velocity()
	local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.bullet_velocity, my_ship.guns.main.bullet_ttl)
	fire("main", a)
	yield()
end

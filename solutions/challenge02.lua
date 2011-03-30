function target_selector(k,c)
	return c:team() ~= team
end

local contacts = sensor_contacts({})
local tid, t = pick(contacts, target_selector)

while true do
	t = sensor_contact(t:id())
	local x, y = position()
	local tx, ty = t:position()
	drive_towards(tx,ty,100)
	fire("main", angle_between(x,y,tx,ty))
	yield()
end

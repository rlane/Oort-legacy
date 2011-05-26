function target_selector(k,c)
	return c:team() ~= team
end

local contacts = sensor_contacts({})
local tid, t = pick(contacts, target_selector)

while true do
	if t ~= nil then
		t = sensor_contact(t:id())

		if t ~= nil then
			local x, y = position()
			local tx, ty = t:position()
			drive_towards(100, tx, ty)
			fire("main", angle_between(x,y,tx,ty))
		end
	end
	yield()
end

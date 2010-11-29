function target_selector(k,c)
	return c:team() ~= team()
end

local contacts = sensor_contacts()
local tid, t = pick(contacts, target_selector)

while true do
	t = sensor_contact(t:id())
	local x, y = position()
	local tx, ty = t:position()
	local a = angle_between(x,y,tx,ty)
	thrust(a,1)
	fire("main", a)
	yield()
end

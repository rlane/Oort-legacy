dofile("examples/lib.lua")

thrust(math.pi/2, 1)
sleep(32)

local i = math.random(1,256)
local t = nil
local bullet_lifetime = 0.5
local bullet_speed = 40
local max_target_distance = bullet_speed*bullet_lifetime
local orbit_x = 0
local orbit_y = 0
local target_selector = function(k,c) return c.team == enemy_team() and c.class ~= "little_missile" end
local target_id = nil

local function fire_score(c)
	local x,y = position()
	if c.team ~= enemy_team() then
		return math.huge
	elseif c.id == target_id then
		return 0
	else
		return distance(x,y,c.x,c.y)
	end
end

while true do
	local msg = recv()
	if msg then
		print("msg: " .. msg)
	end

	local contacts = sensor_contacts()
	local target
	if target_id then target = contacts[target_id] end

	if not target then
		target_id, target = pick(contacts, target_selector)
	end

	local x, y = position()

	local follow
	if distance(x,y,0,0) < 50 and target then
		follow = target
	else
		follow = { x = 0, y = 0, vx = 0, vy = 0 }
	end

	i = i + 1
	if i >= 256 then
		i = 0
	end

	local vx, vy = velocity()

	if i % 8 == 0 then
		local t2 = min_by(contacts, fire_score)
		if t2 then
			local a2 = lead(x, y, t2.x, t2.y, vx, vy, t2.vx, t2.vy, bullet_speed, bullet_lifetime)
			if a2 then
				fire("main", a2+R(-0.04,0.04))
			end
		end
	end

	local a = lead(x, y, follow.x, follow.y, vx, vy, follow.vx, follow.vy, 10, math.huge)
	if a then
		local k = math.random(10)
		if k < 7 then
			local f = math.min(5, 1.0*math.sqrt(distance(x, y, follow.x, follow.y)))
			local nvx = vx + f * math.cos(a)
			local nvy = vy + f * math.sin(a)
			--if distance(0, 0, nvx, nvy) < 10 then
				thrust(a, f)
			--end
		elseif k < 8 then
			a = normalize_angle(a + R(0.5,1) * sign(R(-1,1)))
			thrust(a, 5)
		else
			a = normalize_angle(a + (math.pi/2) * sign(R(-1,1)))
			thrust(a, 5)
		end
	else
		local a = angle_between(vx, vy, 0, 0)
		thrust(a, 10)
	end

	yield()
end

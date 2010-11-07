dofile("examples/lib.lua")

--print("orders:", orders)
local my_team = team()
local my_ship = ships[class()]

thrust(math.pi/2, 1)
sleep(32)

local i = math.random(1,256)
local t = nil
local max_target_distance = my_ship.guns.main.bullet_velocity*my_ship.guns.main.bullet_ttl
local origin = { x = 0, y = 0, vx = 0, vy = 0 }
local target_selector = function(k,c) return c:team() ~= my_team and c:class() ~= "little_missile" end
local follow_target = nil
local follow_target_retry = 0
local fire_target = nil
local fire_target_retry = 0

local function fire_score(c)
	local x,y = position()
	if c:team() == my_team then
		return math.huge
	elseif c:id() == target_id then
		return 0
	else
		return distance(x,y,c:position())
	end
end

while true do
	local msg = recv()
	if msg then
		print("msg: " .. msg)
	end

	clear_debug_lines()
	local x, y = position()
	local vx, vy = velocity()

	if not follow_target and follow_target_retry == 16 then
		local contacts = sensor_contacts()
		follow_target = pick(contacts, target_selector)
	elseif not follow_target then
		follow_target_retry = follow_target_retry + 1
	else
		follow_target = sensor_contact(follow_target:id())
	end

	local follow
	if follow_target and distance(x,y,0,0) < 50 then
		follow = follow_target
	else
		follow = nil
	end

	local follow_x, follow_y
	local follow_vx, follow_vy
	if follow == nil then
		follow_x, follow_y = 0, 0
		follow_vx, follow_vy = 0, 0
	else
		follow_x, follow_y = follow:position()
		follow_vx, follow_vy = follow:velocity()
	end
	debug_square(follow_x, follow_y, 0.5)

	if not fire_target and fire_target_retry == 16 then
		fire_target = min_by(sensor_contacts(), fire_score)
		fire_target_retry = 0
	elseif not fire_target then
		fire_target_retry = fire_target_retry + 1
	else
		fire_target = sensor_contact(fire_target:id())
	end

	if fire_target then
		local tx, ty = fire_target:position()
		local tvx, tvy = fire_target:velocity()
		debug_diamond(tx, ty, 0.5)
		local a = lead(x, y, tx, ty, vx, vy, tvx, tvy, my_ship.guns.main.bullet_velocity, my_ship.guns.main.bullet_ttl)
		if a then
			local spread = 0.04
			fire("main", a+R(-spread,spread))
		else
			fire_target = nil
		end
		--debug_box(t.x-1, t.y-1, t.x+1, t.y+1)
	else
		--debug_box_off()
	end

	local a = lead(x, y, follow_x, follow_y, vx, vy, follow_vx, follow_vy, 10, math.huge)
	if a then
		local k = math.random(10)
		if k < 7 then
			local f = math.min(5, 1.0*math.sqrt(distance(x, y, follow_x, follow_y)))
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

	if follow_target and math.random(1000) == 7 then
		spawn("little_missile", "examples/little_missile.lua", serialize_id(follow_target:id()))
	end

	yield()
end

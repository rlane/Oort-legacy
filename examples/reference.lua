local my_class = class
local my_ship = ships[my_class]

-- Target to follow and focus fire on
local primary_target = nil
local primary_target_counter = 0

-- Target of opportunity
local secondary_target = nil
local secondary_target_counter = 0


function main_loop()
	while true do
		clear_debug_lines()
		do_radio()
		pick_primary_target()
		weapons_ai()
		engines_ai()
		do_construction()
		yield()
	end
end


---- Targeting

-- Lower values mean this ship is more likely to engage other ships of that class
priorities = {
	fighter = {
		fighter = 0.5,
		carrier = 2,
		ion_cannon_frigate = 1,
		assault_frigate = 1.5,
	},
	carrier = {
		fighter = 2,
		carrier = 1.2,
		ion_cannon_frigate = 1,
		assault_frigate = 0.5,
	},
	ion_cannon_frigate = {
		fighter = 10,
		carrier = 0.7,
		ion_cannon_frigate = 0.5,
		assault_frigate = 1.0,
	},
	assault_frigate = {
		fighter = 8,
		carrier = 1.2,
		ion_cannon_frigate = 1,
		assault_frigate = 0.5,
	},
}

my_priorities = priorities[my_class]

function primary_target_scorer(c)
	local p = position_vec()
	local a = angle_between_vec(p, c:position_vec())
	local class_score = my_priorities[c:class()] or 10.0
	local heading_score = math.abs(angle_diff(heading(), a))/math.pi
	local distance_score = clamp(0, 1, math.log(p:distance(c:position_vec()))/3)
	local rand_score = math.random()
	if c:position_vec():length() > scenario_radius then return math.huge end
	return 0.1*heading_score +
	       0.2*distance_score +
	       0.4*class_score +
	       0.2*rand_score
end

-- We will move towards the primary target
function pick_primary_target()
	if primary_target then
		primary_target = sensor_contact(primary_target:id())
		if primary_target and
		   not (primary_target:position_vec():length() > scenario_radius) then
			local p = primary_target:position_vec()
			debug_square(p.x, p.y, 2*my_ship.radius)
			return
		end
		primary_target_counter = 0
	end

	if primary_target_counter == 0 then
		primary_target = min_by(sensor_contacts{ enemy = true }, primary_target_scorer)
		primary_target_counter = 16
		if primary_target then
			log("picked primary target class %s", primary_target:class())
		end
	else
		primary_target_counter = primary_target_counter - 1
	end
end

function secondary_target_scorer(c)
	-- Concentrate fire on the primary target
	if primary_target and c:id() == primary_target:id() then
		return 0
	end

	local p = position_vec()
	local a = angle_between_vec(p,c:position_vec())
	local class_score = my_priorities[c:class()] or 10.0
	local heading_score = math.abs(angle_diff(heading(), a))/math.pi
	local rand_score = math.random()
	return 0.6*heading_score +
	       0.3*class_score +
	       0.1*rand_score
end

-- The secondary target needs to be within weapons range
function pick_secondary_target(max_dist)
	if secondary_target then
		secondary_target = sensor_contact(secondary_target:id())
		if secondary_target then
			local p = secondary_target:position_vec()
			debug_diamond(p.x, p.y, 2*my_ship.radius)
			return
		end
		secondary_target_counter = 0
	end

	if secondary_target_counter == 0 then
		secondary_target = min_by(sensor_contacts{ enemy = true, distance_lt = max_dist }, secondary_target_scorer)
		secondary_target_counter = 16
		if primary_target then
			log("picked secondary target class %s", primary_target:class())
		end
	else
		secondary_target_counter = secondary_target_counter - 1
	end
end


---- Handle radio messages

function do_radio()
	local msg = recv()
	while msg do
		local cmd, arg = msg:match("(%a+)%z(.+)")

		-- Send some reaction mass to the requesting ship
		if cmd == "refuel" and my_ship.guns.refuel then
			local refuel_target_id = arg
			local refuel_range = my_ship.guns.refuel.velocity*my_ship.guns.refuel.ttl
			local refuel_target = sensor_contact(refuel_target_id)
			if refuel_target then
				fire_at_contact("refuel", refuel_target)
			end
		end

		msg = recv()
	end
end


---- Build ships

function do_construction()
	-- Only carriers can build things other than missiles
	if my_class ~= "carrier" then
		return
	end

	if energy() > my_ship.energy.limit*0.1 and chance(3) then
		spawn("fighter", "")
	end
end


---- Weapons AI

weapons = {}

function weapons.carrier()
	point_defense("laser")
	pick_secondary_target(2e3)

	if primary_target and chance(3) then
		spawn("torpedo", primary_target:id())
	end

	if secondary_target and chance(1) then
		spawn("missile", secondary_target:id())
	end
end

function weapons.assault_frigate()
	point_defense("laser")
	pick_secondary_target(my_ship.guns.main.velocity*my_ship.guns.main.ttl*1.5)

	if primary_target and chance(1) then
		spawn("missile", primary_target:id())
	end

	if secondary_target then
		fire_at_contact("main", secondary_target)
	end
end

function weapons.ion_cannon_frigate()
	if primary_target then
		local p = position_vec()
		local tp = primary_target:position_vec()
		local bearing = angle_between_vec(p, tp)
		local da = angle_diff(heading(),bearing)
		local dist = p:distance(tp)
		if dist <= my_ship.guns.main.length and math.abs(da) < 0.1 then
			fire("main", heading())
		end
	end
end

function weapons.fighter()
	pick_secondary_target(my_ship.guns.main.velocity*my_ship.guns.main.ttl*1.2)

	if primary_target and chance(2) then
		log("firing a missile at primary target")
		spawn("missile", primary_target:id())
	end

	if secondary_target then
		fire_at_contact("main", secondary_target)
	end
end

-- Fire short-range defensive weapon at missiles
function point_defense(gun_name)
	local range = my_ship.guns[gun_name].length
	local t = sensor_contacts{ distance_lt = range, enemy = true, class = "torpedo", limit = 1 }[1] or
	          sensor_contacts{ distance_lt = range, enemy = true, class = "missile", limit = 1 }[1]

	-- Shoot at any enemy if we have spare energy
	if not t and energy() > 0.1*my_ship.energy.limit then
		t = sensor_contacts{ distance_lt = range, enemy = true, limit = 1 }[1]
	end

	if t then
		fire_at_contact(gun_name, t)
	end
end


---- Engines AI

engines = {}

function engines.carrier()
end

function engines.assault_frigate()
	move_to_primary_target(100)
	if reaction_mass() < 0.1*my_ship.reaction_mass then move_to_carrier() end
	if reaction_mass() < 0.01*my_ship.reaction_mass then request_refuel() end
end

function engines.ion_cannon_frigate()
	move_to_primary_target(100)
	if reaction_mass() < 0.1*my_ship.reaction_mass then move_to_carrier() end
	if reaction_mass() < 0.01*my_ship.reaction_mass then request_refuel() end
end

function engines.fighter()
	move_to_primary_target(300)
	if reaction_mass() < 0.01*my_ship.reaction_mass then explode() end
end


---- Utility functions

-- Send a radio message telling all carriers to send us reaction mass
local last_refuel_time = 0
function request_refuel()
  if last_refuel_time + 10 < time() then
    send("refuel\0" .. id)
    last_refuel_time = time()
  end
end

-- Move to a carrier
function move_to_carrier()
	local carrier = sensor_contacts{ enemy=false, class="carrier", limit=1 }[1]
	if carrier then drive_towards_vec(50, carrier:position_vec()) end
end

-- Move to the primary target, or if none then the origin
function move_to_primary_target(speed)
	if primary_target then
		drive_towards_vec(speed, primary_target:position_vec())
	else
		drive_towards_vec(speed/2, vec(0, 0))
	end
end


---- Configure AI for this ship's class

weapons_ai = weapons[my_class]
engines_ai = engines[my_class]

---- Run AI

if my_class == "missile" or my_class == "torpedo" then
	standard_missile_ai()
else
	main_loop()
end

ships = {}

ships.fighter = {
	radius = 4/32,
	hull = 1,
	guns = {
		main = {
			bullet_mass = 0.010,
			bullet_velocity = 40,
			bullet_ttl = 0.5,
			reload_time = 0.10,
		}
	}
}

local flak = {
	bullet_mass = 0.1,
	bullet_velocity = 25,
	bullet_ttl = 0.5,
	reload_time = 0.15,
}

ships.mothership = {
	radius = 10/32,
	hull = 1000,
	guns = {
		main = {
			bullet_mass = 6,
			bullet_velocity = 5,
			bullet_ttl = 10,
			reload_time = 1.0,
		},
		flak1 = flak,
		flak2 = flak,
		flak3 = flak,
	}
}

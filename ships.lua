ships = {}

ships.fighter = {
	radius = 4/32,
	hull = 10,
	guns = {
		main = {
			bullet_mass = 0.05,
			bullet_velocity = 20,
			bullet_ttl = 0.5,
			reload_time = 0.15,
		}
	},
	explosion = {
		count = 16,
		velocity = 2,
		ttl = 1,
	},
	count_for_victory = true
}

local flak = {
	bullet_mass = 0.01,
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
	},
	explosion = {
		count = 256,
		velocity = 2,
		ttl = 2,
	},
	count_for_victory = true
}

ships.missile = {
	radius = 1/32,
	hull = 1,
	guns = {},
	explosion = {
		count = 128,
		mass = 0.1,
		velocity = 4,
		ttl = 0.5,
	},
	count_for_victory = false
}

ships.little_missile = {
	radius = 0.5/32,
	hull = 0.1,
	guns = {},
	explosion = {
		count = 16,
		mass = 0.01,
		velocity = 2,
		ttl = 1,
	},
	count_for_victory = false
}

ships = {}

ships.fighter = {
	radius = 4/32,
	hull = 10,
	cost = 200,
	guns = {
		main = {
			bullet_mass = 0.05,
			bullet_velocity = 20,
			bullet_ttl = 0.5,
			reload_time = 0.15,
			cost = 1,
		}
	},
	explosion = {
		count = 16,
		velocity = 2,
		ttl = 1,
	},
	count_for_victory = true,
	energy = {
		initial = 50,
		rate = 10,
		limit = 100,
	}
}

local flak = {
	bullet_mass = 0.01,
	bullet_velocity = 25,
	bullet_ttl = 0.5,
	reload_time = 0.15,
	cost = 1,
}

ships.mothership = {
	radius = 10/32,
	hull = 1000,
	cost = 2000,
	guns = {
		main = {
			bullet_mass = 6,
			bullet_velocity = 5,
			bullet_ttl = 10,
			reload_time = 1.0,
			cost = 20,
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
	count_for_victory = true,
	energy = {
		initial = 500,
		rate = 100,
		limit = 1000,
	}
}

ships.missile = {
	radius = 1/32,
	hull = 0.2,
	cost = 40,
	guns = {},
	explosion = {
		count = 128,
		mass = 0.1,
		velocity = 4,
		ttl = 0.5,
	},
	count_for_victory = false,
	energy = {
		initial = 20,
		rate = 0,
		limit = 20,
	}
}

ships.little_missile = {
	radius = 0.5/32,
	hull = 0.1,
	cost = 20,
	guns = {},
	explosion = {
		count = 32,
		mass = 0.01,
		velocity = 2,
		ttl = 1,
	},
	count_for_victory = false,
	energy = {
		initial = 10,
		rate = 0,
		limit = 10,
	}
}

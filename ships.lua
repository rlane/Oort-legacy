bullets = {
	slug = 1,
	plasma = 2,
	explosion = 3,
}

ships = {}

ships.fighter = {
	radius = 4/32,
	hull = 100,
	max_acc = 10,
	cost = 200,
	guns = {
		main = {
			bullet_type = bullets.slug,
			bullet_mass = 0.001,
			bullet_velocity = 50,
			bullet_ttl = 0.2,
			spread = 0.1,
			reload_time = 0.03,
			cost = 0.3,
		}
	},
	explosion = {
		count = 16,
		mass = 0.1,
		velocity = 2,
		ttl = 1,
	},
	count_for_victory = true,
	energy = {
		initial = 20,
		rate = 8,
		limit = 50,
	},
	spawnable = { 'little_missile' }
}

local flak = {
	bullet_mass = 0.01,
	bullet_velocity = 25,
	bullet_ttl = 0.5,
	bullet_type = bullets.slug,
	spread = 0.2,
	reload_time = 0.15,
	cost = 1,
}

ships.mothership = {
	radius = 1,
	hull = 10000,
	max_acc = 2,
	cost = 2000,
	guns = {
		main = {
			bullet_mass = 10,
			bullet_velocity = 5,
			bullet_ttl = 10,
			bullet_type = bullets.plasma,
			spread = 0.03,
			reload_time = 1.0,
			cost = 20,
		},
		flak1 = flak,
		flak2 = flak,
		flak3 = flak,
	},
	explosion = {
		count = 256,
		mass = 1,
		velocity = 2,
		ttl = 2,
	},
	count_for_victory = true,
	energy = {
		initial = 500,
		rate = 100,
		limit = 1000,
	},
	spawnable = { 'little_missile', 'missile', 'fighter' }
}

ships.missile = {
	radius = 1/32,
	hull = 1.0,
	max_acc = 10,
	cost = 60,
	guns = {},
	explosion = {
		count = 128,
		mass = 0.1,
		velocity = 40,
		ttl = 0.5,
	},
	count_for_victory = false,
	energy = {
		initial = 20,
		rate = 0,
		limit = 20,
	},
	spawnable = {}
}

ships.little_missile = {
	radius = 0.5/32,
	hull = 0.5,
	max_acc = 20,
	cost = 40,
	guns = {},
	explosion = {
		count = 64,
		mass = 0.01,
		velocity = 10,
		ttl = 1,
	},
	count_for_victory = false,
	energy = {
		initial = 10,
		rate = 0,
		limit = 10,
	},
	spawnable = {}
}

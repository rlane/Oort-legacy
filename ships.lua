ships = {}

ships.fighter = {
	radius = 4/32,
	hull = 1,
	guns = {
		main = {
			bullet_mass = 0.1,
			bullet_velocity = 20,
			bullet_ttl = 1,
			reload_time = 0.25,
		}
	}
}

ships.mothership = {
	radius = 10/32,
	hull = 100,
	guns = {
		turret1 = {
			bullet_mass = 0.5,
			bullet_velocity = 10,
			bullet_ttl = 5,
			reload_time = 0.35,
		},
		turret2 = {
			bullet_mass = 0.5,
			bullet_velocity = 10,
			bullet_ttl = 5,
			reload_time = 0.35,
		},
		turret3 = {
			bullet_mass = 0.5,
			bullet_velocity = 10,
			bullet_ttl = 5,
			reload_time = 0.35,
		},
		turret4 = {
			bullet_mass = 0.5,
			bullet_velocity = 10,
			bullet_ttl = 5,
			reload_time = 0.35,
		},
	}
}

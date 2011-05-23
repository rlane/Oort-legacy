local pi = 3.141592653589793

bullets = {
	slug = 1,
	plasma = 2,
	explosion = 3,
	refuel = 4,
}

beams = {
	ion = 1,
	laser = 2,
}

ships = {}

ships.fighter = {
	radius = 10,
	mass = 10e3,
	reaction_mass = 20e3,
	refuelable = true,
	hull = 450e3,
	max_main_acc = 100,
	max_lateral_acc = 10,
	max_angular_acc = 1,
	cost = 10e9,
	guns = {
		main = {
			type = "bullet",
			graphics = bullets.slug,
			mass = 0.001,
			radius = 1.0/32,
			velocity = 3000,
			ttl = 0.2,
			spread = 0.1,
			angle = 0.0,
			coverage = 0.8*pi,
			reload_time = 0.03,
			cost = 4.5e3,
		}
	},
	count_for_victory = true,
	energy = {
		initial = 5e9,
		rate = 5e9,
		limit = 15e9,
	},
	spawnable = {
		missile = 5,
	}
}

ships.ion_cannon_frigate = {
	radius = 40,
	mass = 160e3,
	reaction_mass = 40e3,
	refuelable = true,
	hull = 10e6,
	max_main_acc = 20,
	max_lateral_acc = 2,
	max_angular_acc = 0.5,
	cost = 30e9,
	guns = {
		main = {
			type = "beam",
			graphics = beams.ion,
			damage = 1.6e6,
			length = 1e3,
			width = 6,
			angle = 0.0,
			coverage = 0.0,
			cost = 30e9,
		}
	},
	count_for_victory = true,
	energy = {
		initial = 10e9,
		rate = 17e9,
		limit = 20e9,
	},
	spawnable = {
	}
}

ships.assault_frigate = {
	radius = 40,
	mass = 160e3,
	reaction_mass = 40e3,
	refuelable = true,
	hull = 15e6,
	max_main_acc = 20,
	max_lateral_acc = 4,
	max_angular_acc = 0.7,
	cost = 30e9,
	guns = {
		main = {
			type = "bullet",
			graphics = bullets.plasma,
			mass = 1.0,
			radius = 1.0/32,
			velocity = 600,
			ttl = 5.0,
			spread = 0.02,
			angle = 0.0,
			coverage = 2*pi,
			reload_time = 0.6,
			cost = 1e9,
		},
		laser = {
			type = "beam",
			graphics = beams.laser,
			damage = 41e3,
			length = 300,
			width = 4,
			angle = 0,
			coverage = 2*pi,
			cost = 10e9,
		}
	},
	count_for_victory = true,
	energy = {
		initial = 10e9,
		rate = 17e9,
		limit = 20e9,
	},
	spawnable = {
		missile = 3,
	}
}

ships.carrier = {
	radius = 80,
	mass = 1e6,
	reaction_mass = 1e6,
	refuelable = true,
	hull = 20e6,
	max_main_acc = 10,
	max_lateral_acc = 1,
	max_angular_acc = 0.1,
	cost = 50e9,
	guns = {
		laser = {
			type = "beam",
			graphics = beams.laser,
			damage = 41e3,
			length = 500,
			width = 4,
			angle = 0,
			coverage = 2*pi,
			cost = 20e9,
		},
		refuel = {
			type = "bullet",
			graphics = bullets.refuel,
			mass = 10e3,
			radius = 10,
			velocity = 200,
			ttl = 60.0,
			spread = 0.02,
			angle = 0.0,
			coverage = 2*pi,
			reload_time = 2,
			cost = 200e6,
			refuel = true,
		},
	},
	count_for_victory = true,
	energy = {
		initial = 30e9,
		rate = 10e9,
		limit = 100e9,
	},
	spawnable = {
		missile = 1,
		torpedo = 2,
		fighter = 5
	}
}

ships.torpedo = {
	radius = 2.5,
	hull = 20e3,
	mass = 1e3,
	reaction_mass = 400,
	refuelable = false,
	max_main_acc = 100,
	max_lateral_acc = 20,
	max_angular_acc = 1.5,
	cost = 11e9,
	guns = {},
	warhead = 1e6,
	count_for_victory = false,
	energy = {
		initial = 10e9,
		rate = 0,
		limit = 10e9,
	},
	spawnable = {}
}

ships.missile = {
	radius = 1.0,
	mass = 200,
	reaction_mass = 80,
	refuelable = false,
	hull = 3e3,
	max_main_acc = 300,
	max_lateral_acc = 150,
	max_angular_acc = 3,
	cost = 3e9,
	guns = {},
	warhead = 300e3,
	count_for_victory = false,
	energy = {
		initial = 2e9,
		rate = 0,
		limit = 2e9,
	},
	spawnable = {}
}

ships.small_target = {
	radius = 10.0,
	mass = 1,
	reaction_mass = 1000,
	refuelable = false,
	hull = 100e3,
	max_main_acc = 1000,
	max_lateral_acc = 1000,
	max_angular_acc = 3,
	cost = 0,
	guns = {},
	count_for_victory = true,
	energy = {
		initial = 1e12,
		rate = 1e12,
		limit = 1e12,
	},
	spawnable = {}
}

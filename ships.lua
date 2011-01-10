-- Reactor is simulated deuterium+tritium fusion with 0.375% mass to energy
-- conversion. Thrust is generated by expelling dedicated reaction mass using
-- energy stored in the battery. Currently the depletion of reaction mass is
-- not simulated. The hydrogen fuel required is tiny compared to the mass of
-- the spacecraft and so is not simulated. The exhaust velocity is currently
-- set at 10 km/s but should be changed to an argument to the thrust()
-- function. 10 km/s is a relatively low exhaust velocity that lowers energy
-- use at the expense of using more reaction mass.

-- Energy/mass requirement for a fighter at full thrust for 1 second:
-- t = 1 s
-- M = 30000 kg (fighter mass)
-- a = 100 m/s^2 (slightly better than a space shuttle main engine bolted on to the fighter)
-- V = 100 m/s
-- v = 10000 m/s (exhaust velocity)
-- m*v = M*V
-- m = M*V/v = 300 kg = 1% of fighter's total mass
-- E = 0.5*m*v^2 = 15 GJ
-- Size the battery for 5 seconds of full thrust:
-- E_b = 15 GW * 5s = 75 GJ
-- Allow continuous thrust at 1/3 power:
-- P = 5 GW

bullets = {
	slug = 1,
	plasma = 2,
	explosion = 3,
}

ships = {}

ships.fighter = {
	radius = 10,
	mass = 30e3,
	hull = 450e3,
	max_acc = 100,
	cost = 10e9,
	guns = {
		main = {
			bullet_type = bullets.slug,
			bullet_mass = 0.001,
			bullet_velocity = 3000,
			bullet_ttl = 0.2,
			spread = 0.1,
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
	spawnable = { 'little_missile' }
}

local flak = {
	bullet_mass = 0.01,
	bullet_velocity = 2000,
	bullet_ttl = 0.5,
	bullet_type = bullets.slug,
	spread = 0.2,
	reload_time = 0.15,
	cost = 20e3,
}

ships.mothership = {
	radius = 80,
	mass = 2e6,
	hull = 4.5e6,
	max_acc = 10,
	cost = 50e9,
	guns = {
		main = {
			bullet_mass = 10,
			bullet_velocity = 300,
			bullet_ttl = 20,
			bullet_type = bullets.plasma,
			spread = 0.03,
			reload_time = 2,
			cost = 0.45e6,
		},
		flak1 = flak,
		flak2 = flak,
		flak3 = flak,
	},
	count_for_victory = true,
	energy = {
		initial = 50e9,
		rate = 5e9,
		limit = 100e9,
	},
	spawnable = { 'little_missile', 'missile', 'fighter' }
}

ships.missile = {
	radius = 2.5,
	hull = 1.0,
	mass = 500,
	max_acc = 100,
	cost = 10e9,
	guns = {},
	warhead = 40,
	count_for_victory = false,
	energy = {
		initial = 10e9,
		rate = 0,
		limit = 10e9,
	},
	spawnable = {}
}

ships.little_missile = {
	radius = 1.0,
	mass = 100,
	hull = 0.5,
	max_acc = 150,
	cost = 3e9,
	guns = {},
	warhead = 30,
	count_for_victory = false,
	energy = {
		initial = 3e9,
		rate = 0,
		limit = 3e9,
	},
	spawnable = {}
}

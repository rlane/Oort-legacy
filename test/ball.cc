#include "test/testcase.h"

class BallAI : public CxxAI {
public:
	BallAI(Ship &ship) : CxxAI(ship) {}

	void tick() {
		auto t = find_target(ship);

		if (t) {
			drive_towards(ship, t->get_position(), ship.klass.max_main_acc*5);

			if (&ship.klass == fighter.get() || &ship.klass == assault_frigate.get()) {
				const GunDef &gun = ship.klass.guns[0];
				auto a = lead(ship.get_position(), t->get_position(),
											ship.get_velocity(), t->get_velocity(),
											gun.velocity, gun.ttl);
				if (!isnan(a)) {
					ship.fire_gun(0, a);
				}

				if (ship.game->ticks % 32 == 0) {
					ship.fire_missile(t);
				}
			} else if (&ship.klass == ion_cannon_frigate.get()) {
				const BeamDef &beam = ship.klass.beams[0];
				float a = angle_between(ship.get_position(), t->get_position());
				float da = angle_diff(ship.get_heading(), a);
				if (fabsf(da) < 0.1 && length(ship.get_position() - t->get_position()) < beam.length*1.1f) {
					ship.fire_beam(0, ship.get_heading());
				}
			} else if (&ship.klass == missile.get()) {
				if (length(ship.get_position() - t->get_position()) < 100) {
					ship.explode();
				}
			}
		} else {
			drive_towards(ship, vec2(0,0), ship.klass.max_main_acc*2);
		}
	}
};

class BallTest : public Test {
public:
	BallTest() {
		boost::random::mt19937 prng(42);
		boost::random::normal_distribution<float> p_dist(0.0, 1000.0);
		boost::random::normal_distribution<float> v_dist(0.0, 20.0);
		boost::uniform_real<float> h_dist(0.0, 2*pi);

		auto ai_factory = CxxAI::factory<BallAI>();
		auto red = make_shared<Team>("red", ai_factory, vec3(1, 0, 0));
		auto green = make_shared<Team>("green", ai_factory, vec3(0, 1, 0));
		auto blue = make_shared<Team>("blue", ai_factory, vec3(0, 0, 1));
		vector<shared_ptr<Team>> teams = { red, green, blue };
		vector<ShipClass*> klasses = { 
			fighter.get(),
			fighter.get(),
			fighter.get(),
			missile.get(),
			ion_cannon_frigate.get(),
			assault_frigate.get(),
		};

		for (auto i = 0; i < 100; i++) {
			auto ship = make_shared<Ship>(this, *klasses[(i/teams.size()) % klasses.size()], teams[i % teams.size()]);
			ship->set_position(vec2(p_dist(prng), p_dist(prng)));
			ship->set_velocity(vec2(v_dist(prng), v_dist(prng)));
			ship->set_heading(h_dist(prng));
			ships.push_back(ship);
		}
	}

	void after_tick() {
		if (ships.empty() || check_victory()) {
			test_finished = true;
		}
	}
} test;

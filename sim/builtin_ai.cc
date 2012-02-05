#include "sim/builtin_ai.h"
#include "test/testcase.h"

namespace Oort {

class BuiltinAI : public CxxAI {
public:
	ProportionalNavigator nav;
	boost::random::mt19937 prng;

	BuiltinAI(Ship &ship)
		: CxxAI(ship),
		  nav(ship, 5, ship.klass.max_main_acc),
	    prng(ship.id) {}

	void tick() {
		auto t = find_target(ship);

		if (t) {
			if (&ship.klass == fighter.get() || &ship.klass == assault_frigate.get()) {
				boost::uniform_real<> gun_dist(0, 3);
				boost::uniform_real<> missile_dist(0, 256);
				drive_towards(ship, t->get_position(), ship.klass.max_main_acc*5);

				if (gun_dist(prng) < 1.0) {
					const GunDef &gun = ship.klass.guns[0];
					auto a = lead(ship.get_position(), t->get_position(),
												ship.get_velocity(), t->get_velocity(),
												gun.velocity, gun.ttl);
					if (!isnan(a)) {
						ship.fire_gun(0, a);
					}
				}

				if (missile_dist(prng) < 1.0) {
					ship.fire_missile(t);
				}
			} else if (&ship.klass == ion_cannon_frigate.get()) {
				drive_towards(ship, t->get_position(), ship.klass.max_main_acc*5);
				const BeamDef &beam = ship.klass.beams[0];
				float a = angle_between(ship.get_position(), t->get_position());
				float da = angle_diff(ship.get_heading(), a);
				if (fabsf(da) < 0.1 && length(ship.get_position() - t->get_position()) < beam.length*1.1f) {
					ship.fire_beam(0, ship.get_heading());
				}
			} else if (&ship.klass == missile.get()) {
				float f = ship.game->time - ship.creation_time;
				if (f < 1) {
					turn_towards(ship, t->get_position());
				} else {
					nav.seek(t->get_position(), t->get_velocity());
				}
			}
		} else {
			drive_towards(ship, vec2(0,0), ship.klass.max_main_acc*2);
		}
	}
};

static CxxAIFactory<BuiltinAI> builtin_ai_factory_obj;
std::shared_ptr<AIFactory> builtin_ai_factory(static_cast<AIFactory*>(&builtin_ai_factory_obj));

}

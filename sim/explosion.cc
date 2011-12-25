#include "sim/explosion.h"
#include <Box2D/Box2D.h>
#include "sim/game.h"
#include "sim/entity.h"
#include "sim/ship.h"
#include "sim/math_util.h"

namespace Oort {

class ExplosionCB : public b2RayCastCallback {
public:
	b2Fixture *fixture;
	glm::vec2 point;

	ExplosionCB() : fixture(NULL) {}

	float ReportFixture(b2Fixture *fixture,
	                    const b2Vec2 &point,
	                    const b2Vec2 &normal,
	                    float32 fraction) {
		this->fixture = fixture;
		this->point = b2n(point);
		return fraction;
	}
};

void Explosion::tick(Game &game) {
	const int n = 32;
	auto &world = *game.world;
	for (int i = 0; i < n; i++) {
		ExplosionCB cb;
		auto a = 2*pi*i/n;
		auto dp = glm::vec2(cosf(a), sinf(a)) * 100.0f;
		world.RayCast(&cb, n2b(p), n2b(p+dp));
		if (cb.fixture) {
			auto entity = (Entity*) cb.fixture->GetBody()->GetUserData();
			auto ship = dynamic_cast<Ship*>(entity);
			if (ship) {
				game.hits.emplace_back(Hit{ ship, NULL, cb.point, e/n });
			}
		}
	}
}

}

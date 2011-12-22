// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_H_
#define OORT_SIM_SHIP_H_

#include <stdint.h>
#include <memory>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include "glm/glm.hpp"
#include "sim/entity.h"

namespace Oort {

class Game;
class AI;
class ShipClass;

enum {
	INVALID_SHIP_ID = (uint32_t)-1
};

class Ship : public Entity {
	public:
	const ShipClass &klass;
	std::unique_ptr<AI> ai;
	uint32_t id;
	float hull;

	Ship(Game *game, const ShipClass &klass, std::shared_ptr<Team> team);
	~Ship();

	virtual void tick();

	void fire_gun(int idx, float angle);
	void fire_beam(int idx, float angle);

	void acc_main(float acc);
	void acc_lateral(float acc);
	void acc_angular(float acc); // rad/s^2

	private:
	float main_acc;
	float lateral_acc;
	float angular_acc;
	boost::random::mt19937 prng;
	std::vector<float> last_fire_times;

	void update_forces();
};

}

#endif

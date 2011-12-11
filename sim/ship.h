// Copyright 2011 Rich Lane
#ifndef OORT_SIM_SHIP_H_
#define OORT_SIM_SHIP_H_

#include <stdint.h>
#include <memory>
#include "sim/entity.h"

namespace Oort {

class Game;
class AI;
class ShipClass;

class Ship : public Entity {
	public:
	const ShipClass *klass;
	std::unique_ptr<AI> ai;
	uint32_t id;

	Ship(Game *game, const ShipClass *klass, std::shared_ptr<Team> team);
	~Ship();

	virtual void tick();
	void fire(float angle);
	void thrust_main(float force);
	void thrust_lateral(float force);
	void thrust_angular(float force);

	private:
	float main_thrust;
	float lateral_thrust;
	float angular_thrust;

	void update_forces();
};

}

#endif

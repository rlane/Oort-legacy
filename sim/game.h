#ifndef OORT_SIM_GAME_H_
#define OORT_SIM_GAME_H_

#include <list>
#include <memory>
#include "sim/ship.h"

namespace Oort {

class Game {
	public:
		Game();
		~Game();
	
		std::list<std::shared_ptr<Ship>> ships;

};

}

#endif

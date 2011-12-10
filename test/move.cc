static std::shared_ptr<Ship> ship;
static std::unique_ptr<Waypoint> start_waypoint;
static std::unique_ptr<Waypoint> middle_waypoint;
static std::unique_ptr<Waypoint> finish_waypoint;

Game *test_init() {
	AISourceCode ai{"foo.lua", ""};
	std::vector<AISourceCode> ais{ ai };
	Scenario scn;
	auto team = make_shared<Team>("green", ai, vec3(0, 1, 0));
	auto game = new Game(scn, ais);
	ship = make_shared<Ship>(game, team);
	game->ships.push_back(ship);
	start_waypoint = unique_ptr<Waypoint>(new Waypoint(game, vec2(0, 0), 0.1));
	middle_waypoint = unique_ptr<Waypoint>(new Waypoint(game, vec2(50, 0), 0.1));
	finish_waypoint = unique_ptr<Waypoint>(new Waypoint(game, vec2(100, 0), 0.1));
	return game;
}

void test_tick(Game *game) {
	const int s = 32;
	const float thrust = 100 * ship->body->GetMass();
	if (game->ticks == 0) {
		assert_contact(*ship, *start_waypoint);
		ship->thrust_main(thrust);
	} else if (game->ticks == 1*s) {
		assert_contact(*ship, *middle_waypoint);
		ship->thrust_main(-thrust);
	} else if (game->ticks == 2*s) {
		assert_contact(*ship, *finish_waypoint);
		ship->thrust_main(0);
		game->test_finished = true;
	}
}

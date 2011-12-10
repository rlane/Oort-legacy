static std::shared_ptr<Ship> ship;

Game *test_init() {
	AISourceCode ai{"foo.lua", ""};
	std::vector<AISourceCode> ais{ ai };
	Scenario scn;
	auto team = make_shared<Team>("green", ai, vec3(0, 1, 0));
	auto game = new Game(scn, ais);
	ship = make_shared<Ship>(game, team);
	game->ships.push_back(ship);
	return game;
}

void test_tick(Game *game) {
	const int s = 32;
	const float thrust = 10 * ship->body->GetMass();
	if (game->ticks == 0) {
		assert_equal(ship->body->GetPosition(), b2Vec2(0, 0));
		ship->thrust_main(thrust);
	} else if (game->ticks == 1*s) {
		assert_equal(ship->body->GetPosition(), b2Vec2(5, 0));
		ship->thrust_main(-thrust);
	} else if (game->ticks == 2*s) {
		assert_equal(ship->body->GetPosition(), b2Vec2(10, 0));
		ship->thrust_main(0);
		game->test_finished = true;
	}
}

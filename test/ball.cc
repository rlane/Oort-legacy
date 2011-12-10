static boost::random::mt19937 prng(42);
static boost::random::normal_distribution<> a_dist(0.0, 10.0);

Game *test_init() {
	AISourceCode ai{"foo.lua", ""};
	Scenario scn;
	boost::random::mt19937 prng(42);
	boost::random::normal_distribution<> p_dist(0.0, 10.0);
	boost::random::normal_distribution<> v_dist(0.0, 5.0);

	scn.teams = {
		{ "red", vec3(1, 0, 0), nullptr, {} },
		{ "green", vec3(0, 1, 0), nullptr, {} },
		{ "blue", vec3(0, 0, 1), nullptr, {} },
	};

	for (auto i = 0; i < 100; i++) {
		ScnShip ship;
		ship.klass = "fighter";
		ship.p = vec2(p_dist(prng), p_dist(prng));
		ship.v = vec2(v_dist(prng), v_dist(prng));
		ship.h = 0.0;
		scn.teams[i % scn.teams.size()].ships.push_back(ship);
	}

	std::vector<AISourceCode> ais{ ai, ai, ai };
	return new Game(scn, ais);
}

void test_tick(Game *game) {
	if (game->ticks == 32*60) {
		game->test_finished = true;
	}

	BOOST_FOREACH(auto ship, game->ships) {
		ship->thrust_main(10);
		ship->thrust_lateral(0);

		auto t = ship->body->GetTransform();
		auto w = ship->body->GetAngularVelocity();
		auto th = angle_between(t.p, b2Vec2(0,0));
		auto dh = angle_diff(t.q.GetAngle(), th);
		ship->thrust_angular(dh - w);

		if (ship->game->ticks % 4 == 0) {
			ship->fire(th);
		}
	}
}

class BallTest : public Test {
public:
	BallTest() {
		boost::random::mt19937 prng(42);
		boost::random::normal_distribution<> p_dist(0.0, 10.0);
		boost::random::normal_distribution<> v_dist(0.0, 5.0);

		AISourceCode ai{"foo.lua", ""};
		auto red = make_shared<Team>("red", ai, vec3(1, 0, 0));
		auto green = make_shared<Team>("green", ai, vec3(0, 1, 0));
		auto blue = make_shared<Team>("blue", ai, vec3(0, 0, 1));
		vector<shared_ptr<Team>> teams = { red, green, blue };

		for (auto i = 0; i < 100; i++) {
			auto ship = make_shared<Ship>(this, fighter, teams[i % teams.size()]);
			ship->body->SetTransform(b2Vec2(p_dist(prng), p_dist(prng)), 0);
			ship->body->SetLinearVelocity(b2Vec2(v_dist(prng), v_dist(prng)));
			ships.push_back(ship);
		}
	}

	void after_tick() {
		if (ticks == 32*60) {
			test_finished = true;
		}

		BOOST_FOREACH(auto ship, ships) {
			ship->thrust_main(10);
			ship->thrust_lateral(0);

			auto t = ship->body->GetTransform();
			auto w = ship->body->GetAngularVelocity();
			auto th = angle_between(t.p, b2Vec2(0,0));
			auto dh = angle_diff(t.q.GetAngle(), th);
			ship->thrust_angular(dh - w);

			if (ticks % 4 == 0) {
				ship->fire(th);
			}
		}
	}
} test;

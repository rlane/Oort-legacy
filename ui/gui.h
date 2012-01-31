namespace Oort {

class GUI {
public:
	enum class State {
		RUNNING,
		FINISHED
	};

	static constexpr float zoom_const = 2.0;
	static constexpr float pan_const = 0.01;
	static constexpr float fps = 60;
	static constexpr float tps = 32;
	static constexpr int target_tick_time = 1000000LL/tps;

	enum State state;
	bool running;
	bool paused;
	bool single_step;
	bool render_physics_debug;
	float view_radius;
	float zoom_rate;
	glm::vec2 view_center;
	glm::vec2 view_speed;
	uint32_t picked_id;
	std::shared_ptr<Game> game;
	Test *test;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<PhysicsDebugRenderer> physics_debug_renderer;
	int screen_width, screen_height;
	float instant_frame_time;
	float instant_tick_time;
	uint64_t last_tick_time;
	boost::mutex mutex;
	float last_time_delta;
	uint64_t prev;
	int tick_count;
	int frame_count;

	GUI(std::shared_ptr<Game> game, Test *test)
		: state(State::RUNNING),
		  running(true),
		  paused(true),
		  single_step(false),
		  render_physics_debug(false),
		  view_radius(3000.0f),
		  zoom_rate(0),
		  view_center(0, 0),
		  view_speed(0, 0),
		  picked_id(INVALID_SHIP_ID),
		  game(game),
			test(test),
			screen_width(0),
			screen_height(0),
			instant_frame_time(0),
			instant_tick_time(0),
			last_tick_time(0),
			last_time_delta(0),
			prev(microseconds()),
			frame_count(0)
	{
		renderer = std::unique_ptr<Renderer>(new Renderer());
		renderer->tick(*game);
		last_tick_time = microseconds();

		physics_debug_renderer = std::unique_ptr<PhysicsDebugRenderer>(new PhysicsDebugRenderer());
		physics_debug_renderer->SetFlags(b2Draw::e_shapeBit);
		game->world->SetDebugDraw(physics_debug_renderer.get());
	}

	void handle_keydown(int sym) {
		switch (sym) {
		case SDLK_ESCAPE:
			running = false;
			break;
		case SDLK_SPACE:
			paused = !paused;
			break;
		case SDLK_RETURN:
			paused = false;
			single_step = true;
			break;
		case SDLK_g:
			render_physics_debug = !render_physics_debug;
			break;
		case SDLK_w:
			view_speed.y += pan_const;
			break;
		case SDLK_s:
			view_speed.y -= pan_const;
			break;
		case SDLK_a:
			view_speed.x -= pan_const;
			break;
		case SDLK_d:
			view_speed.x += pan_const;
			break;
		case SDLK_z:
			zoom_rate -= zoom_const;
			break;
		case SDLK_x:
			zoom_rate += zoom_const;
			break;
		case SDLK_b:
			renderer->benchmark = !renderer->benchmark;
			break;
		default:
			break;
		}
	}

	void handle_keyup(int sym) {
		switch (sym) {
		case SDLK_w:
			view_speed.y -= pan_const;
			break;
		case SDLK_s:
			view_speed.y += pan_const;
			break;
		case SDLK_a:
			view_speed.x += pan_const;
			break;
		case SDLK_d:
			view_speed.x -= pan_const;
			break;
		case SDLK_z:
			zoom_rate += zoom_const;
			break;
		case SDLK_x:
			zoom_rate -= zoom_const;
			break;
		default:
			break;
		}
	}

	class PickCallback : public b2QueryCallback {
	public:
		b2Vec2 center;
		uint32_t found_id;

		PickCallback(b2Vec2 center) 
		: center(center),
			found_id(INVALID_SHIP_ID) {}

		bool ReportFixture(b2Fixture *fixture) {
			auto body = fixture->GetBody();
			auto entity = static_cast<Entity*>(body->GetUserData());
			auto ship = dynamic_cast<Ship*>(entity);
			if (ship && !ship->dead && fixture->TestPoint(center)) {
				found_id = ship->id;
			}
			return true;
		}
	};

	void handle_mousebuttondown(int button, int x, int y) {
		if (button == 1) {
			auto c = screen2world(glm::vec2(x,y));
			glm::vec2 size(1,1);
			b2AABB aabb;
			aabb.lowerBound = n2b(c - size);
			aabb.upperBound = n2b(c + size);
			PickCallback picker(n2b(c));
			game->world->QueryAABB(&picker, aabb);
			if (picker.found_id != INVALID_SHIP_ID) {
				picked_id = picker.found_id;
			} else {
				picked_id = INVALID_SHIP_ID;
			}
		}
	}

	void handle_resize(int w, int h) {
		screen_width = w;
		screen_height = h;
		printf("resize %d %d\n", w, h);
		glViewport(0, 0, w, h);
		renderer->reshape(w, h);
		physics_debug_renderer->reshape(w, h);
	}

	glm::vec2 screen2world(glm::vec2 screen_pos) {
		screen_pos -= glm::vec2(screen_width, screen_height) * 0.5f;
		screen_pos.y = -screen_pos.y;
		return view_center + screen_pos * (2*view_radius/screen_width);
	}

	glm::vec2 mouse_position() {
		int x, y;
		SDL_GetMouseState(&x, &y);
		return glm::vec2(x, y);
	}

	void render() {
		auto frame_start = microseconds();

		if (zoom_rate < 0) {
			auto p = screen2world(mouse_position());
			auto dp = (zoom_const/fps) * (p - view_center);
			view_center += dp;
		}
		view_radius *= (1 + zoom_rate/fps);

		view_center += view_speed*view_radius;

		if (paused) {
			std::ostringstream tmp;
			tmp << "(paused) tick " << game->ticks;
			renderer->text(screen_width-tmp.str().length()*9-3, screen_height-22, tmp.str());
		} else {
			std::ostringstream tmp;
			tmp << "tick " << game->ticks;
			renderer->text(screen_width-tmp.str().length()*9-3, screen_height-22, tmp.str());
		}

		if (state == State::RUNNING) {
			renderer->text(screen_width-120, screen_height-10, "test running");
		} else if (state == State::FINISHED) {
			renderer->text(screen_width-120, screen_height-10, "test finished");
		} 

		if (picked_id != INVALID_SHIP_ID) {
			auto ship = game->lookup_ship(picked_id);
			if (ship && !ship->dead) {
				const int x = 15;
				const int dy = 12;
				const int y = screen_height - 5*dy - 3;
				std::ostringstream tmp;
				auto p = ship->get_position();
				auto h = ship->get_heading();
				auto v = ship->get_velocity();
				tmp << ship->klass.name << " " << ship->id;
				renderer->text(x, y+0*dy, tmp.str()); tmp.str("");
				tmp << "position: (" << p.x << "," << p.y << ")";
				renderer->text(x, y+1*dy, tmp.str()); tmp.str("");
				tmp << "heading: " << h;
				renderer->text(x, y+2*dy, tmp.str()); tmp.str("");
				tmp << "velocity: (" << v.x << "," << v.y << ")";
				renderer->text(x, y+3*dy, tmp.str()); tmp.str("");
				tmp << "hull: " << ship->hull;
				renderer->text(x, y+4*dy, tmp.str()); tmp.str("");
			} else {
				picked_id = INVALID_SHIP_ID;
			}
		}

		if (renderer->benchmark) {
			renderer->text(screen_width-160, 10, boost::str(boost::format("render: %0.2f ms") % instant_frame_time));
			renderer->text(screen_width-160, 20, boost::str(boost::format("  tick: %0.2f ms") % instant_tick_time));
		}

		{
			boost::lock_guard<boost::mutex> lock(mutex);

			if (render_physics_debug) {
				auto p = screen2world(mouse_position());
				std::ostringstream tmp;
				tmp << "mouse position: " << glm::to_string(p);
				renderer->text(5, 9, tmp.str());
			}

			float time_delta = float(microseconds() - last_tick_time)/(1000*1000);
			if (time_delta > 0.1f) time_delta = 0;
			renderer->render(view_radius, view_center, paused ? last_time_delta : time_delta);
			if (!paused) {
				last_time_delta = time_delta;
			}

			if (render_physics_debug) {
				physics_debug_renderer->begin_render(view_radius, view_center);
				for (const b2Body *body = game->world->GetBodyList(); body; body = body->GetNext()) {
					physics_debug_renderer->DrawPoint(body->GetWorldCenter(), 2, b2Color(0.9, 0.4, 0.3));
					physics_debug_renderer->DrawPoint(body->GetPosition(), 2, b2Color(0.3, 0.4, 0.9));
				}
				physics_debug_renderer->DrawCircle(b2Vec2(0,0), game->radius/Oort::SCALE, b2Color(0.6,0.8,0.6));
				game->world->DrawDebugData();
				physics_debug_renderer->end_render();
			}
		}

		++frame_count;
		auto now = microseconds();
		auto elapsed = now - prev;
		instant_frame_time = float(now - frame_start)/1000;
		if (elapsed >= 1000000LL) {
			printf("%0.2f fps\n", 1e6*frame_count/elapsed);
			frame_count = 0;
			prev = now;
			if (renderer->benchmark) {
				renderer->dump_perf();
			}
		}
	}

	static void static_ticker_func(void *arg) {
		auto gui = static_cast<GUI*>(arg);
		gui->ticker_func();
	}

	void ticker_func() {
		while (running) {
			auto tick_start = microseconds();

			if (!paused) {
				if (single_step) {
					single_step = false;
					paused = true;
				}

				if (state == State::RUNNING) {
					if (test->finished) {
						printf("Test finished\n");
						state = State::FINISHED;
					}
				}

				game->tick();
				test->after_tick();
				{
					boost::lock_guard<boost::mutex> lock(mutex);
					renderer->tick(*game);
					last_tick_time = microseconds();
				}
			}

			auto tick_end = microseconds();
			int tick_time = tick_end - tick_start;
			instant_tick_time = float(tick_time)/1000;
			if (tick_time < target_tick_time) {
				usleep(target_tick_time - tick_time);
			}
		}
	}
};

}

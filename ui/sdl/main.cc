// Copyright 2011 Rich Lane
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include "gl/gl.h"
#include <SDL.h>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "sim/math_util.h"
#include "common/log.h"
#include "common/resources.h"
#include "sim/ship.h"
#include "sim/ship_class.h"
#include "sim/game.h"
#include "sim/scenario.h"
#include "sim/team.h"
#include "renderer/renderer.h"
#include "renderer/physics_debug_renderer.h"
#include "sim/test.h"

using glm::vec2;
using std::make_shared;
using std::shared_ptr;
using boost::format;
using boost::str;

namespace Oort {

glm::vec2 screen2world(glm::vec2 screen_pos);

static enum class State {
	RUNNING,
	FINISHED
} state = State::RUNNING;

static bool running = true;
static bool paused = true;
static bool single_step = false;
static bool render_physics_debug = false;
static float view_radius = 3000.0f;
static float zoom_rate = 0;
static const float zoom_const = 2.0;
static glm::vec2 view_center;
static glm::vec2 view_speed;
static const float pan_const = 0.01;
static int screen_width = 800, screen_height = 600;
static const float fps = 60;
static const float tps = 32;
static const int target_tick_time = 1000000LL/tps;
static uint32_t picked_id = INVALID_SHIP_ID;
static shared_ptr<Test> game;
static float instant_frame_time = 0.0f;
static float instant_tick_time = 0.0f;
static boost::mutex mutex;

static std::unique_ptr<Renderer> renderer;
static std::unique_ptr<PhysicsDebugRenderer> physics_debug_renderer;

static void handle_keydown(int sym) {
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
	default:
		break;
	}
}

static void handle_keyup(int sym) {
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

static void handle_mousebuttondown(int button, int x, int y) {
	if (button == 1) {
		auto c = screen2world(vec2(x,y));
		vec2 size(1,1);
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

static void handle_resize(int w, int h) {
	screen_width = w;
	screen_height = h;
	SDL_SetVideoMode(w, h, 32, SDL_OPENGL | SDL_RESIZABLE);
	glViewport(0, 0, w, h);
	renderer->reshape(w, h);
	physics_debug_renderer->reshape(w, h);
}

static void handle_sdl_event(const SDL_Event &event) {
	switch(event.type) {
		case SDL_KEYDOWN:
			handle_keydown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			handle_keyup(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			handle_mousebuttondown(event.button.button, event.button.x, event.button.y);
			break;
		case SDL_VIDEORESIZE:
			handle_resize(event.resize.w, event.resize.h);
			break;
		case SDL_QUIT:
			running = false;
			break;
	}
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

void ticker_func() {
	while (running) {
		auto tick_start = microseconds();
		game->tick();
		{
			boost::lock_guard<boost::mutex> lock(mutex);
			renderer->tick(*game);
		}
		auto tick_end = microseconds();
		int tick_time = tick_end - tick_start;
		instant_tick_time = float(tick_time)/1000;
		if (tick_time < target_tick_time) {
			usleep(target_tick_time - tick_time);
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s test.so\n", argv[0]);
		return 1;
	}

	ShipClass::initialize();

	printf("Running test %s\n", argv[1]);
	game = Test::load(std::string(argv[1]));

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1) != 0) {
		printf("unable to configure vsync\n");
	}

	SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL | SDL_RESIZABLE);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	renderer = std::unique_ptr<Renderer>(new Renderer());
	renderer->tick(*game);

	physics_debug_renderer = std::unique_ptr<PhysicsDebugRenderer>(new PhysicsDebugRenderer());
	physics_debug_renderer->SetFlags(b2Draw::e_shapeBit);
	game->world->SetDebugDraw(physics_debug_renderer.get());

	handle_resize(screen_width, screen_height);

	auto prev = microseconds();
	int frame_count = 0;

	boost::thread ticker(ticker_func);

	while (running) {
		auto frame_start = microseconds();

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			handle_sdl_event(event);
		}

		if (!paused) {
			if (single_step) {
				single_step = false;
				paused = true;
			}

			if (state == State::RUNNING) {
				if (game->test_finished) {
					printf("Test finished\n");
					state = State::FINISHED;
				}
			}
		}

		if (zoom_rate < 0) {
			auto p = screen2world(mouse_position());
			auto dp = (zoom_const/fps) * (p - view_center);
			view_center += dp;
		}
		view_radius *= (1 + zoom_rate/fps);

		view_center += view_speed*view_radius;

		{
			boost::lock_guard<boost::mutex> lock(mutex);
			renderer->render(view_radius, view_center);
		}

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

		if (render_physics_debug) {
			physics_debug_renderer->begin_render(view_radius, view_center);
			for (const b2Body *body = game->world->GetBodyList(); body; body = body->GetNext()) {
				physics_debug_renderer->DrawPoint(body->GetWorldCenter(), 2, b2Color(0.9, 0.4, 0.3));
				physics_debug_renderer->DrawPoint(body->GetPosition(), 2, b2Color(0.3, 0.4, 0.9));
			}
			physics_debug_renderer->DrawCircle(b2Vec2(0,0), game->radius/Oort::SCALE, b2Color(0.6,0.8,0.6));
			game->world->DrawDebugData();
			physics_debug_renderer->end_render();
			{
				auto p = screen2world(mouse_position());
				std::ostringstream tmp;
				tmp << "mouse position: " << glm::to_string(p);
				renderer->text(5, 9, tmp.str());
			}
		}

		renderer->text(screen_width-160, 10, str(format("render: %0.2f ms") % instant_frame_time));
		renderer->text(screen_width-160, 20, str(format("  tick: %0.2f ms") % instant_tick_time));

		SDL_GL_SwapBuffers();

		++frame_count;
		auto now = microseconds();
		auto elapsed = now - prev;
		instant_frame_time = float(now - frame_start)/1000;
		if (elapsed >= 1000000LL) {
			printf("%0.2f fps\n", 1e6*frame_count/elapsed);
			frame_count = 0;
			prev = now;
			renderer->dump_perf();
		}
	}

	ticker.join();

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}

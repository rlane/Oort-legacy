// Copyright 2011 Rich Lane
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <GL/glew.h>
#include <SDL.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#include <boost/timer.hpp>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

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

// needs -lrt (real-time lib)
// 1970-01-01 epoch UTC time, 1 mcs resolution (divide by 1M to get time_t)
uint64_t ClockGetTime()
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec / 1000LL;
}

namespace Oort {

glm::vec2 screen2world(glm::vec2 screen_pos);

static enum class State {
	RUNNING,
	FINISHED
} state = State::RUNNING;

static bool running = true;
static bool paused = false;
static bool single_step = false;
static bool render_physics_debug = false;
static float view_radius = 100.0f;
static float zoom_rate = 0;
static const float zoom_const = 2.0;
static glm::vec2 view_center;
static glm::vec2 view_speed;
static const float pan_const = 0.01;
static const int screen_width = 1600, screen_height = 900;
static const float fps = 60;
static uint32_t picked_id = INVALID_SHIP_ID;
static shared_ptr<Test> game;

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
		if (ship != nullptr && fixture->TestPoint(center)) {
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

typedef int (*test_main_ft)();

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

	SDL_SetVideoMode(screen_width, screen_height, 16, SDL_OPENGL | SDL_FULLSCREEN);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glViewport(0, 0, screen_width, screen_height);
	float aspect_ratio = float(screen_width)/screen_height;

	SDL_Event event;

	renderer = std::unique_ptr<Renderer>(new Renderer(game));

	physics_debug_renderer = std::unique_ptr<PhysicsDebugRenderer>(new PhysicsDebugRenderer());
	physics_debug_renderer->SetFlags(b2Draw::e_shapeBit);
	game->world->SetDebugDraw(physics_debug_renderer.get());

	auto prev = ClockGetTime();
	int frame_count = 0;

	while (running) {
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

			game->tick();
		}

		if (zoom_rate < 0) {
			auto p = screen2world(mouse_position());
			auto dp = (zoom_const/fps) * (p - view_center);
			view_center += dp;
		}
		view_radius *= (1 + zoom_rate/fps);

		view_center += view_speed*view_radius;

		renderer->render(view_radius, aspect_ratio, view_center);

		if (state == State::RUNNING) {
			renderer->text(8, screen_height-10, "test running");
		} else if (state == State::FINISHED) {
			renderer->text(8, screen_height-10, "test finished");
		} 

		if (picked_id != INVALID_SHIP_ID) {
			std::ostringstream tmp;
			tmp << "picked " << picked_id;
			renderer->text(8, 4, tmp.str());
		}

		if (render_physics_debug) {
			physics_debug_renderer->begin_render(view_radius, aspect_ratio, view_center);
			for (const b2Body *body = game->world->GetBodyList(); body; body = body->GetNext()) {
				physics_debug_renderer->DrawPoint(body->GetWorldCenter(), 2, b2Color(0.9, 0.4, 0.3));
				physics_debug_renderer->DrawPoint(body->GetPosition(), 2, b2Color(0.3, 0.4, 0.9));
			}
			physics_debug_renderer->DrawCircle(b2Vec2(0,0), 30.0f, b2Color(0.6,0.8,0.6));
			game->world->DrawDebugData();
			physics_debug_renderer->end_render();
		}

		SDL_GL_SwapBuffers();

		++frame_count;
		auto now = ClockGetTime();
		auto elapsed = now - prev;
		if (elapsed >= 1000000LL) {
			printf("%0.2f fps\n", 1e6*frame_count/elapsed);
			frame_count = 0;
			prev = now;
		}
	}

	return 0;
}

}

int main(int argc, char **argv) {
	return Oort::main(argc, argv);
}

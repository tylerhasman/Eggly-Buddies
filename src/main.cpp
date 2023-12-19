
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "systems/core/physics_system.hpp"
#include "systems/core/render_system.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/gameplay/animation_system.h"
#include "systems/gameplay/save_system.h"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	WorldSystem world_system;
	RenderSystem render_system;
	PhysicsSystem physics_system;
	AnimationSystem animationSystem;

    GameplaySystems::initializeGameplaySystems(&world_system, &render_system);

	// Initializing window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

    

	// initialize the main systems
	render_system.init(window, world_system.getCamera());
	world_system.init(&render_system);
	SaveSystem::load();
	world_system.updateBackyardText();
	// variable timestep loop
	auto t = Clock::now();
	while (!world_system.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

        // Cap delta
        if(elapsed_ms > 1000.0f / 60.0f){
            elapsed_ms = 1000.0f / 60.0f;
        }

        GameplaySystems::step(elapsed_ms);

        world_system.step(elapsed_ms);

		physics_system.step(elapsed_ms);

		world_system.handle_collisions();

        animationSystem.step(elapsed_ms);

		render_system.draw();
	}
	
	SaveSystem::saveBuddyStatus();
    GameplaySystems::destroyGameplaySystems();

	return EXIT_SUCCESS;
}

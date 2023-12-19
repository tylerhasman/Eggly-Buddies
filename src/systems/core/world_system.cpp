// Header
#include "world_system.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/rooms/intro_room.h"
#include "systems/rooms/living_room.h"
#include "systems/rooms/backyard.h"
#include "systems/rooms/backyard_game_room.h"
#include "systems/rooms/bedroom.h"
#include "systems/rooms/dream_game_room.h"
#include "systems/rooms/bath_room.h"
#include "systems/rooms/bathroom_game_room.h"
#include "systems/rooms/kitchen_room.h"
#include "systems/rooms/kitchen_game_room.h"
#include "systems/rooms/end_room.h"
#include "systems/rooms/mesh_test_room.h"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/ai/buddy_state_machine.h"


// stlib
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace std;

// Create the fish world
WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (dream_game_music != nullptr)
		Mix_FreeMusic(dream_game_music);
	if (ui_click_sound != nullptr)
		Mix_FreeChunk(ui_click_sound);
	if (jump_sound != nullptr)
		Mix_FreeChunk(jump_sound);
	if (death_sound != nullptr)
		Mix_FreeChunk(death_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);

    for(auto room : rooms){
        delete room;
    }
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char* desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

    // Get primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == NULL) {
        fprintf(stderr, "Failed to initialize GLFW, no monitors found");
        return nullptr;
    }

    // Get current video mode
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode == NULL) {
        fprintf(stderr, "Failed to initialize GLFW, no video mode");
        return nullptr;
    }

    // Now, mode->width and mode->height contain the dimensions of the monitor in pixels
    //int monitor_width = (int) (mode->width);
    int monitor_height = (int) (mode->height) - 100;

/*    window_width_px = monitor_width;
    window_height_px = (int) (((float) monitor_width) * 2.0f / 3.0f);*/

    window_height_px = monitor_height;
    window_width_px = (int) (window_height_px * 3.0f / 2.0f);
	
    std::cout << "Window size set to " << window_width_px << "x" << window_height_px << std::endl;

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Eggly Buddies", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_callback = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("backgroundMusic.wav").c_str());
	dream_game_music = Mix_LoadMUS(audio_path("dreamMusic.wav").c_str());
	ui_click_sound = Mix_LoadWAV(audio_path("uiClick.wav").c_str());
	jump_sound = Mix_LoadWAV(audio_path("jump.wav").c_str());
	death_sound = Mix_LoadWAV(audio_path("death.wav").c_str());

	if (background_music == nullptr || ui_click_sound == nullptr || jump_sound == nullptr || death_sound == nullptr
		|| dream_game_music == nullptr) {
		fprintf(stderr, "Failed to load sound");
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// set this val to true if game.json doesn't exist, else set to false
	string name = string(PROJECT_SOURCE_DIR) + "/data/game.json";
	ifstream f(name.c_str());
    if (f.good()) {
		isIntroCutScene = false;
	} else {
		isIntroCutScene = true;
	}
	
    if (isIntroCutScene) {
        restart_game();
		displayUI(false);
        intro_cut_scene();
    } else {
        introSystem->introStep = IntroStep::IntroFinished;
		displayUI(true);
        restart_game();
    };
}

// Update our game world
bool WorldSystem::step(float elapsed_ms) {
    displayRoomTimer -= elapsed_ms / 1000.0f;

	if (isIntroCutScene && introSystem->introStep == IntroStep::IntroFinished) {
		changeRoom(0, 0);
		isIntroCutScene = false;
		mailSystem->mailState = MailState::TimerTicking;
	}

	registry.renderRequests.sort(WorldSystem::compareByZIndex);

	keyInputUpdate();

    updateCameraPosition(elapsed_ms);

    kitchenGameSystem->playerInput(isLeftHeld, isRightHeld);

    AiController& buddyAiController = registry.aiControllers.get(buddy);
    RenderRequest& buddyRenderRequest = registry.renderRequests.get(buddy);
    SpriteSheet& buddySpriteSheet = registry.spriteSheets.get(buddy);
    Motion& buddyMotion = registry.motions.get(buddy);

    auto state = buddyAiController.stateMachine.getCurrentState();

    auto buddyState = std::dynamic_pointer_cast<BuddyState>(state);

    buddyMotion.scale = buddyStatusSystem->getBuddySize();

    if(buddyState){

        BuddyActionType action = buddyState->getBuddyAction();

        if(!buddyState->isBuddyOnGround() && action != BuddyActionType::Sleep){
            action = BuddyActionType::Jump;
        }

        buddyStatusSystem->getBuddyLook(action, buddyRenderRequest.used_texture, buddySpriteSheet);

        BuddyDirection direction = buddyState->getBuddyDirection();

        if(direction == BuddyDirection::Right){
            buddyMotion.scale.x = glm::abs(buddyMotion.scale.x);
        }else{
            buddyMotion.scale.x = -glm::abs(buddyMotion.scale.x);
        }

    }

	for (Entity entity : registry.cleanTools.entities) {
		// progress timer
		auto tool = registry.cleanTools.get(entity);
		if (tool.timer <= 0.0f) {

			if (registry.cleanTools.has(entity)){
				registry.remove_all_components_of(entity);
			}
		}
	}

	return true;
}

void WorldSystem::intro_cut_scene() {
	std::cout << "starting intro cut scene" << std::endl;
    introSystem->introStep = IntroStep::IntroStarted;
	mailSystem->mailState = MailState::Pause;

    displayRoomTimer = 3.0f;
	createRoom<IntroRoom>(0, 1);
	
	// display intro
	introSystem->showIntroEvent();

	std::cout << "created intro cut scene" << std::endl;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {

	// Debugging for memory/component leaks
	registry.list_all_components();

    GameplaySystems::destroyGameplaySystems();
    GameplaySystems::initializeGameplaySystems(this, renderer);

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

    GameplaySystems::destroyGameplaySystems();
    GameplaySystems::initializeGameplaySystems(this, renderer);

    lastRoom = vec2(0.0);

	if (isIntroCutScene) {
    	currentRoom = vec2(0, 1);
	} else {
		currentRoom = vec2(0.0);
	};

    roomSwitchAlpha = 1.0;
	isEndScene = false;

    vec2 backgroundSize = vec2(game_width, game_height);
    vec2 backgroundPosition = vec2(game_width / 2.0f, game_height / 2.0f);

    isLeftHeld = false;
    isRightHeld = false;

	Entity mail_icon = create_sprite_entity(renderer, vec2(game_width - 64, 64), vec2(128, 128), Textures::MAIL_ICON, true);

    createBuddy();

    Motion cameraMotion;
    cameraMotion.position = vec2(0.0);

    registry.cameras.emplace(camera);
    registry.motions.emplace(camera, cameraMotion);

    //Sprite sheet example, uncomment to edit and play around with
    //Uses the spritesheet_test.png texture

/*    Entity spriteSheetTest = create_sprite_entity(renderer, vec2(window_width_px, window_height_px) / 2.0f, vec2(128, 128), Textures::SPRITESHEET_TEST, true,-10);

    SpriteSheet spriteSheet;
    spriteSheet.dimensions = vec2(2, 2);// Spritesheet is 2x2
    spriteSheet.current = vec2(0, 0); // Select the top-left sprite, zero-index so the bottom right sprite is (1,1)
    spriteSheet.frameSpeed = 0.5f;

    registry.spriteSheets.emplace(spriteSheetTest, spriteSheet);*/


    createRoom<BathRoom>(-2);
	// createRoom<BathroomGameRoom>(-2, -1);
	// testing for bathroom game room
	createRoom<BathroomGameRoom>(-2,-1); 
    createRoom<Bedroom>(-1);
	createRoom<DreamGameRoom>(-1, -1);
    createRoom<LivingRoom>(0);
	//createRoom<IntroRoom>(0, 1);
	createRoom<KitchenRoom>(1);
	createRoom<KitchenGameRoom>(1, -1);
    createRoom<Backyard>(2);
	createRoom<BackyardGameRoom>(2, -1);
	Backyard::updateFertilizerText();

    //createRoom<MeshTestRoom>(3);
	//bar initializations
	Entity hygieneBar = create_sprite_entity(renderer,  vec2(game_width - 90, 150), vec2(180, 50), Textures::HYGIENE, true);
	Entity restBar = create_sprite_entity(renderer, vec2(game_width - 90,  203), vec2(180, 50), Textures::REST, true);
	Entity satiationBar = create_sprite_entity(renderer, vec2(game_width - 90, 258), vec2(180, 50), Textures::SATIATION, true);
	Entity entertainmentBar = create_sprite_entity(renderer, vec2(game_width -90, 310), vec2(180, 50), Textures::ENTERTAINMENT, true);

	registry.statusBars.emplace(hygieneBar);
	registry.statusBars.emplace(restBar);
	registry.statusBars.emplace(satiationBar);
	registry.statusBars.emplace(entertainmentBar);

	registry.hygiene.emplace(hygieneBar);
	registry.rest.emplace(restBar);
	registry.satiation.emplace(satiationBar);
	registry.entertainment.emplace(entertainmentBar);


    Entity whistle = create_sprite_entity(renderer, vec2(90, 90), vec2(130, 104), Textures::WHISTLE, true);

    Clickable& clickHandler = registry.clickables.emplace(whistle);
    clickHandler.onClick = [this](Entity ent) {
        AiController& controller = registry.aiControllers.get(buddy);

        bool isNoAi = false;

        if(std::dynamic_pointer_cast<BuddyNoAi>(controller.stateMachine.getCurrentState())){
            isNoAi = true;
        }

        if(!isNoAi){
            controller.stateMachine.enterState(BuddyState::States::FollowPlayer);
        }

    };

}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;

	bool isBeingCleaned = false;

	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other_entity;
		//dreamGame collisions


		if (registry.toilets.has(entity) && registry.poops.has(entity_other)) {
			
			Room* room = getRoomOfType<BathRoom>();
			BathRoom* bathroom = dynamic_cast<BathRoom*>(room);
			bathroom->triggerBathroomGame(entity_other);
		}

        if(registry.pickupables.has(entity) && registry.pickupables.has(entity_other)){
            Motion& one = registry.motions.get(entity);
            Motion& two = registry.motions.get(entity_other);
            vec2 dif = glm::normalize(one.position - two.position);

            dif.y *= 10.0f;

            one.velocity = dif;
        }

		//bathroom poop book collisions
		if (registry.books.has(entity) && registry.poops.has(entity_other)) {
			std::cout << "close the book\n";
			bathroomGameSystem->onhitBook();
		}

		if (registry.toilets.has(entity) && registry.poops.has(entity_other)) {
			
			Room* room = getRoomOfType<BathRoom>();
			BathRoom* bathroom = dynamic_cast<BathRoom*>(room);
			bathroom->triggerBathroomGame(entity_other);
		}

		//bathroom poop book collisions
		if (registry.books.has(entity) && registry.poops.has(entity_other)) {
			std::cout << "close the book\n";
			bathroomGameSystem->onhitBook();
		}

		// if (registry.players.has(entity) && registry.fallingFoods.has(entity_other)) {
		// 	kitchenGameSystem->handleCatchFood(entity_other);
		// }

		// float status_refresh_rate = 0.2f;

		if(registry.collisionHandlers.has(entity)){
			CollisionHandler& handler = registry.collisionHandlers.get(entity);

			handler.onCollision(entity, collisionsRegistry.components[i]);
		}

        if(registry.pickupReceivers.has(entity) && registry.pickupables.has(entity_other)){
            PickupReceiver& receiver = registry.pickupReceivers.get(entity);
            if(receiver.onPickupReceived){
                receiver.onPickupReceived(entity_other);
            }
        }

		//CHRIS speeed for demo 
		float status_refresh_rate = .3f;

		if (registry.players.has(entity) && registry.cleanTools.has(entity_other)) {
			RenderRequest& cleaningRequest = registry.renderRequests.get(entity);
			registry.cleanTools.get(entity_other).timer -= 1.f / 100.f;
			cleaningRequest.iUniforms["u_EnableCleaning"] = 1;
			isBeingCleaned = true;
			Motion& motion = registry.motions.get(entity_other);
			float swingMagnitude = 1.0f; 
			float swingSpeed = 0.01f; 
			static float swingDirection = 1.0f; // Starts swinging to one side (1 = one way, -1 = the other)
			motion.angle += swingSpeed * swingDirection;
			// If the motion goes beyond the limits, change direction
			if (motion.angle > swingMagnitude || motion.angle < -swingMagnitude) {
				swingDirection = -swingDirection; // Reverse the swing direction
			}
			

			//update dirtiness value
			//Dirtiness& dirtiness = registry.dirtiness.get(entity);
			//if (dirtiness.value > 0)
			//dirtiness.value -= status_refresh_rate; // being cleaned
			if (buddyStatusSystem->hygiene <= 100) {
				buddyStatusSystem->hygiene += status_refresh_rate;
			}
		}

	}

	if (!isBeingCleaned) {
		for (auto& entity : registry.players.entities) {
			RenderRequest& cleaningRequest = registry.renderRequests.get(entity);
			cleaningRequest.iUniforms["u_EnableCleaning"] = 0;
		}
		for (auto& entity_other : registry.cleanTools.entities) {
			Motion& motion = registry.motions.get(entity_other);
			float resetSpeed = 0.01f; // Adjust as necessary
			if (motion.angle > 0) motion.angle -= resetSpeed;
			if (motion.angle < 0) motion.angle += resetSpeed;
		}
	}

	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R && !isIntroCutScene && !isEndScene) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
    }

    bool minigameActive = isMiniGameActive();

	// Left Right movement
	if (key == GLFW_KEY_LEFT) {
        isLeftHeld = action != GLFW_RELEASE;

        if(!minigameActive && !isIntroCutScene && !isEndScene){
            if(action == GLFW_PRESS && roomSwitchAlpha >= 1.0f){
                changeRoom(currentRoomX - 1, currentRoomY);

                LivingRoom* livingRoom = getRoomOfType<LivingRoom>();

                if(livingRoom){
                    registry.remove_all_components_of(livingRoom->tutorialLeftKey);
                    registry.remove_all_components_of(livingRoom->tutorialRightKey);
                }

            }
        }
	}

	if (key == GLFW_KEY_RIGHT) {
        isRightHeld = action != GLFW_RELEASE;

        if(!minigameActive && !isIntroCutScene && !isEndScene){
            if(action == GLFW_PRESS && roomSwitchAlpha >= 1.0f){
                changeRoom(currentRoomX + 1, currentRoomY);

                LivingRoom* livingRoom = getRoomOfType<LivingRoom>();

                if(livingRoom){
                    registry.remove_all_components_of(livingRoom->tutorialLeftKey);
                    registry.remove_all_components_of(livingRoom->tutorialRightKey);
                }

            }
        }
    }

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (dreamGameSystem->isRunning) {
			dreamGameSystem->jump();
		}
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		if (backyardGameSystem->isGameRunning() && !isIntroCutScene && !isEndScene) {
			for(const Entity& enemy : registry.backyardEnemies.entities){
        		BackyardEnemy& backyardEnemy = registry.backyardEnemies.get(enemy);
				registry.remove_all_components_of(backyardEnemy.imageOverlay);
        		registry.remove_all_components_of(enemy);
    		}
		}
	}

	//start end scene
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (!isIntroCutScene && !isEndScene) {
            if(buddyStatusSystem->growthStage == BuddyGrowthStage::Adult){
                startEndScene();
            }
		}
	}
}

void WorldSystem::keyInputUpdate() {

}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    mousePosition = mouse_position / vec2(window_width_px, window_height_px) * vec2(game_width, game_height);
    mouseWorldPosition = mousePosition + registry.motions.get(camera).position;

	if (backyardGameSystem->isGameRunning()) {
			backyardGameSystem->playerInput(mousePosition + registry.motions.get(camera).position);
	}

    for(Entity& entity : registry.renderRequests.entities){
        RenderRequest& renderRequest = registry.renderRequests.get(entity);
        renderRequest.iUniforms["u_EnableHoverGlow"] = 0;
    }

    for(Entity& entity : registry.clickables.entities){

        Clickable& clickable = registry.clickables.get(entity);
        if(!clickable.highlightOnHover){
            continue;
        }

        if(!registry.renderRequests.has(entity)){
            continue;
        }

        if(registry.motions.has(entity)){
            Motion motion = registry.motions.get(entity);

            vec2 mousePos =  registry.userInterfaces.has(entity) ? mousePosition : mouseWorldPosition;

            if(mousePos.x > motion.position.x - motion.scale.x / 2 && mousePos.x < motion.position.x + motion.scale.x / 2){
                if(mousePos.y > motion.position.y - motion.scale.y / 2 && mousePos.y < motion.position.y + motion.scale.y / 2){
                    RenderRequest& renderRequest = registry.renderRequests.get(entity);
                    renderRequest.iUniforms["u_EnableHoverGlow"] = 1;
                }
            }

        }
    }

}

void WorldSystem::on_mouse_button(int button, int action, int mods) {
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
		bool somethingClicked = false;
		if (backyardGameSystem->isGameRunning()) {
			backyardGameSystem->shoot();
		}
		else if (kitchenGameSystem->isGameRunning() || dreamGameSystem->isRunning || (bathroomGameSystem->isRunning && bathroomGameSystem->userPickTime)) {
				for(Entity& entity : registry.clickables.entities){
					
						if (registry.motions.has(entity)) {
							Motion motion = registry.motions.get(entity);

							vec2 mousePos = registry.userInterfaces.has(entity) ? mousePosition : mouseWorldPosition;

							if (mousePos.x > motion.position.x - motion.scale.x / 2 && mousePos.x < motion.position.x + motion.scale.x / 2) {
								if (mousePos.y > motion.position.y - motion.scale.y / 2 && mousePos.y < motion.position.y + motion.scale.y / 2) {
									on_entity_clicked(entity);
									somethingClicked = true;
								}
							}

						}
					}
				
		}
	else {


			for(Entity& entity : registry.clickables.entities){
				if(registry.motions.has(entity)){
					Motion motion = registry.motions.get(entity);

					vec2 mousePos =  registry.userInterfaces.has(entity) ? mousePosition : mouseWorldPosition;

					if(mousePos.x > motion.position.x - glm::abs(motion.scale.x) / 2 && mousePos.x < motion.position.x + glm::abs(motion.scale.x) / 2){
						if(mousePos.y > motion.position.y - glm::abs(motion.scale.y) / 2 && mousePos.y < motion.position.y + glm::abs(motion.scale.y) / 2){
							on_entity_clicked(entity);
                            somethingClicked = true;
						}
					}

				}
			}
            if(somethingClicked){
                Mix_PlayChannel(-1, ui_click_sound, 0);
            }
		}

		
    }
}

bool WorldSystem::compareByZIndex(Entity a, Entity b) {
	return registry.renderRequests.get(a).z_index > registry.renderRequests.get(b).z_index;
}

void WorldSystem::on_entity_clicked(Entity entity) {

    if(registry.clickables.has(entity)){
        Clickable& clickable = registry.clickables.get(entity);

        if(clickable.onClick){
            clickable.onClick(entity);
        }else{
            std::cout << "Clickable entity has no onClick function set!" << std::endl;
        }
    }

}

float WorldSystem::random() {
    return uniform_dist(rng);
}

Entity WorldSystem::getCamera() {
    return camera;
}

void WorldSystem::updateCameraPosition(float elapsed_ms) {
    vec2 lastRoomPosition = lastRoom * vec2(game_width, game_height);
    vec2 desiredRoomPosition = currentRoom * vec2(game_width, game_height);

    constexpr float exponent = 2.0f;
    float alpha = 1.0f - powf((1.0f - roomSwitchAlpha), exponent);

    vec2 interpolatedPosition = lastRoomPosition + (desiredRoomPosition - lastRoomPosition) * alpha;

    roomSwitchAlpha = glm::min(1.0f, roomSwitchAlpha + elapsed_ms / 1000.0f);

    Motion& cameraPosition = registry.motions.get(camera);

    cameraPosition.position = interpolatedPosition;

    mouseWorldPosition = mousePosition + registry.motions.get(camera).position;
}

void WorldSystem::changeRoom(int slotX, int slotY) {
    if(isRoomValid(slotX, slotY)){
        lastRoom = currentRoom;
        currentRoom = vec2(slotX, slotY);
        roomSwitchAlpha = 0.0f;
        currentRoomX = slotX;
        currentRoomY = slotY;
    }
}

template<class RoomClass>
void WorldSystem::createRoom(int slotX, int slotY) {
    static_assert(std::is_base_of<Room, RoomClass>::value, "RoomClass not derived from Room");

    validRooms.emplace_back(slotX, slotY);

    vec2 roomMin = vec2(slotX * game_width, slotY * game_height);
    vec2 roomMax = vec2((slotX + 1) * game_width, (slotY + 1) * game_height);

	std::cout << "Creating room at x pos:" << roomMin.x << ", " << roomMax.x << std::endl;
	std::cout << "Creating room at y pos:" << roomMin.y << ", " << roomMax.y << std::endl;

    RoomClass* room = new RoomClass(roomMin, roomMax, renderer);

    room->setupRoom(this);

    rooms.push_back(room);
}

bool WorldSystem::isRoomValid(int slotX, int slotY) {
    auto it = std::find_if(validRooms.begin(), validRooms.end(),[slotX, slotY](const std::pair<int,int> pair) {
        return pair.first == slotX && pair.second == slotY;
    });

    return it != validRooms.end();
}

void WorldSystem::displayUI(bool isDisplay) {
	if (!isDisplay) {
		for (Entity entity : registry.userInterfaces.entities) {
			RenderRequest& renderReq = registry.renderRequests.get(entity);
			renderReq.z_index += tempUIZIncrement;
		}
	}
	else {
		for (Entity entity : registry.userInterfaces.entities) {
			RenderRequest& renderReq = registry.renderRequests.get(entity);
			renderReq.z_index -= tempUIZIncrement;
		}
	}
}

void WorldSystem::createBuddy() {
    vec2 backgroundPosition = vec2(game_width / 2.0f, game_height / 2.0f);

	// createSquirrel(renderer, backgroundPosition - vec2(300.0, 256.0f));
	// createSquirrelImage(renderer, backgroundPosition - vec2(300.0, 256.0f));


    buddy = create_sprite_entity(renderer, backgroundPosition - vec2(0.0, -256.0f), vec2(128, 100), Textures::TEST_GUY, false, -2);

    registry.players.emplace(buddy);

    registry.egglystatus.emplace(buddy);
    registry.satiation.emplace(buddy);
    registry.hygiene.emplace(buddy);
    registry.rest.emplace(buddy);
	registry.entertainment.emplace(buddy);

    Text& text = registry.texts.emplace(buddy);
    text.text = "";

    SpriteSheet& spriteSheet = registry.spriteSheets.emplace(buddy);;
    spriteSheet.dimensions = vec2(1, 1);
    spriteSheet.current = vec2(0, 0);
    spriteSheet.frameSpeed = 0.5f;

    AiController& controller = registry.aiControllers.emplace(buddy);

    controller.stateMachine.addState(BuddyState::States::Idle, std::make_shared<BuddyIdleState>(buddy));
    controller.stateMachine.addState(BuddyState::States::WanderRandom, std::make_shared<BuddyWanderRandomState>(buddy));

    //TOOD: Implement
    controller.stateMachine.addState(BuddyState::States::FindRandomPickup, std::make_shared<BuddyIdleState>(buddy));

    controller.stateMachine.addState(BuddyState::States::FollowPlayer, std::make_shared<BuddyFollowPlayer>(buddy));
    controller.stateMachine.addState(BuddyState::States::NoAi, std::make_shared<BuddyNoAi>(buddy));
    controller.stateMachine.enterState(BuddyState::States::Idle);
	controller.stateMachine.addState(BuddyState::States::Poo, std::make_shared<BuddyPoo>(buddy));
	controller.stateMachine.addState(BuddyState::States::Evolve, std::make_shared<BuddyEvolveState>(buddy));
	controller.stateMachine.addState(BuddyState::States::Dead, std::make_shared<BuddyDeathState>(buddy));
    RenderRequest& renderRequest = registry.renderRequests.get(buddy);
    buddyStatusSystem->getBuddyLook(BuddyActionType::Idle, renderRequest.used_texture, spriteSheet);


    Clickable& clickable = registry.clickables.emplace(buddy);
    clickable.highlightOnHover = false;

}

vec2 WorldSystem::getRandomBuddyPosition() {

    std::vector<std::pair<int,int>> walkableRooms;

    for(auto room : validRooms){
        if(room.second == 0){
            walkableRooms.push_back(room);
        }
    }
    std::uniform_int_distribution<> distr(0, (int) walkableRooms.size() - 1);

    int r = distr(rng);

    std::pair<int, int> room = walkableRooms[r];

    float randomRoomX = random() * game_width * 0.8f + game_width * 0.1f;

    vec2 positionInRoom = vec2((float) room.first * game_width + randomRoomX, 0.0f);

    return positionInRoom;
}

bool WorldSystem::isMiniGameActive() {
    return kitchenGameSystem->isGameRunning() || dreamGameSystem->isRunning || backyardGameSystem->isGameRunning() || bathroomGameSystem->isRunning;
}

void WorldSystem::startEndScene() {
	isEndScene = true;
	displayUI(false);
	createRoom<EndRoom>(0, 1);
	changeRoom(0, 1);
	std::cout << "starting end scene" << std::endl;
	mailSystem->mailState = MailState::Pause;
	// display end
	endSystem->showEndEvent(window);
}

void WorldSystem::updateBackyardText() {
	Backyard::updateFertilizerText();
	Backyard::updateBasketText(Backyard::basketEntity);
	// Update other room texts as needed
}

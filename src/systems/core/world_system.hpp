#pragma once

// internal
#include "engine/common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_mixer.h"

#include "render_system.hpp"
#include "systems/rooms/room.h"
#include "systems/gameplay/dream_game_system.h"
#include "systems/gameplay/kitchen_game_system.h"
#include "systems/gameplay/backyard_game_system.h"


// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);


    // Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over() const;

    // Returns a random float between 0.0 and 1.0
    float random();

    Entity getCamera();
	
	// are left or right keys being pressed?
	bool isLeftHeld;
	bool isRightHeld;

    Entity getBuddy(){
        return buddy;
    }

    vec2 mousePosition;

    vec2 mouseWorldPosition;

    void changeRoom(int slotX, int slotY = 0);

    template<class RoomClass>
    void createRoom(int slotX, int slotY = 0);

	// music references
	Mix_Music* background_music;
	Mix_Music* dream_game_music;
	Mix_Chunk* ui_click_sound;
	Mix_Chunk* jump_sound;
	Mix_Chunk* death_sound;

    vec2 getRandomBuddyPosition();

	template <typename RoomType>
	std::pair<vec2, vec2> getRoomPositionByType() {
		for (Room* room : rooms) {
			RoomType* specificRoom = dynamic_cast<RoomType*>(room);
			if (specificRoom != nullptr) {
				// Found a room of the specified type
				return std::make_pair(specificRoom->roomMin, specificRoom->roomMax);
			}
		}
		return std::make_pair(vec2(0, 0), vec2(0, 0)); // Handle the 'not found' case
	}

	template <typename RoomType>
	RoomType* getRoomOfType() {
		for (Room* room : rooms) {
			RoomType* specificRoom = dynamic_cast<RoomType*>(room);
			if (specificRoom != nullptr) {
				return specificRoom; // Found the room of specified type
			}
		}
		return nullptr; // No room of specified type found
	}
	
    void displayUI(bool isDisplay);
    static bool isMiniGameActive();
    // restart level
    void restart_game();
	std::vector<Room*> rooms;
	void updateBackyardText();
    bool isIntroCutScene = true;

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mods);

    void on_entity_clicked(Entity entity);

	bool helpOpened = false;

	void intro_cut_scene();
    float displayRoomTimer = 0.0f;

	// update game state based on key inputs
	void keyInputUpdate();

    void updateCameraPosition(float elapsed_ms);

    bool isRoomValid(int slotX, int slotY = 0);


	// OpenGL window handle
	GLFWwindow* window;

	// Game state
	RenderSystem* renderer;

	Entity buddy;

    Entity camera;

    vec2 lastRoom = vec2(0);
    vec2 currentRoom = vec2(0);

    int currentRoomX = 0;
    int currentRoomY = 0;

    float roomSwitchAlpha = 0.0f;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	static bool compareByZIndex(Entity a, Entity b);

    std::vector<std::pair<int, int>> validRooms;

    int tempUIZIncrement = 200;

    void createBuddy();

	void startEndScene();
	bool isEndScene = false;

};



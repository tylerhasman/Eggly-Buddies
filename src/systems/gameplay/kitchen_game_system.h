#pragma once

#include <random>
#include "systems/core/world_system.hpp"

class KitchenGameSystem {

public:
    KitchenGameSystem(RenderSystem* renderer) : renderer(renderer) {
        isLeftHeld = false;
        isRightHeld = false;
    }
    
    void step(float elapsed_ms);

    void displayInstructions(float elapsed_ms);

    void stopInstructions();
    
    void beginGame();

    void displayEndMsg(bool isWin);

    void endGame();

    void setupKitchenGame(Room* gameRoom, Entity playerEntity);

    bool isGameRunning() const {
        return isRunning;
    }

    void playerInput(bool moveLeft, bool moveRight);

    void handleCatchFood(Entity food, Entity catcher);

    void incrementScore();

    void increaseSatiaton();

    void displayUI(bool isDisplay);

private:
    RenderSystem* renderer;

    float score = 0.f;

    int pancakesCaught = 0;
    int pancakesMissed = 0;

    Room* room;

    Entity plate;

    bool isRunning = false;

    // tutorial
    float curr_instruction_timer = 5000.f;
    const float INSTRUCTION_TIMER = 5000.f; // how long instructions will stay on screen
    bool isShowingInstructions = false;

    std::vector<Entity> fallingFoods;

    float displayMessageTimer = 0.0f;
    float foodFallingTimer = 0.0f;

    bool isLeftHeld, isRightHeld;

    float playerMoveSpeed = 400.0f;

    bool hasPlateBeenStackedOn = false;

    void spawnFallingFood();
    float next_pancake_spawn = 0.f;
    const size_t MAX_PANCAKES = 25;
    const size_t PANCAKE_DELAY_MS = 2000 * 3;
    vec2 pancake_speed = vec2(0.f, 100.f);
    vec2 pancake_size =  vec2(75, 75);

    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist;

    // increase game speed over time
    float gameSpeed = 1.f;

    float MAX_SATIATION_INCREASE = 100.f;

    Entity displayText;

    int tempUIZIncrement = 200;

};

#pragma once

#include <random>
#include "systems/core/world_system.hpp"

class DreamGameSystem {

public:
    DreamGameSystem(RenderSystem* renderer) : renderer(renderer) {

    }

    void step(float elapsed_ms);
    void speedupGame(float elapsed_ms);
    void beginGame();
    void endGame(float elapsed_ms);
    void spawnRandomEnemy(float elapsed_ms);
    void jump();
    void hitEnemy();
    void checkEnemiesAvoided();
    void jumpCheck(float elapsed_ms);
    void applyGravity(float elapsed_ms);
    void addToScore(float toAdd);
    void displayInstructions(float elapsed_ms);
    void stopInstructions();
    void reduceTirednessAddHunger();
    void displayUI(bool isDisplay);
    vec2 roomC = vec2(0.f, 0.f);
    vec2 roomS = vec2(0.f, 0.f);
    vec2 roomMin = vec2(0.f, 0.f);
    WorldSystem* worldSystem = nullptr;
    bool isRunning = false;

    //reference to entities
    //Entity background;
    //Entity ground;

private:
    RenderSystem* renderer;
    float score = 0.f;
    
    bool gameOver = false;
    float playerX = roomS.x / 6.f;
    float playerY = (roomS.y * 3.f / 4.2f);
    float enemyCheckpoint = (roomS.x / 6.f);
    float min_timer_ms = 3000.f;
    float fadeTimer = 3000.f;

    //enemies
    const size_t MAX_ENEMIES = 4;
    const float RABBIT_VALUE = 10.f;
    const float SHEEP_VALUE = 20.f;
    const size_t ENEMY_DELAY_MS = 2000 * 3;
    float next_enemy_spawn = 0.f;
    float rabbit_speed = -480.f;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist;

    //jump variables
    float jumpHeight = 160.f;
    float currPlayerY = (roomS.y * 3.f / 4.2f);
    bool isJumping = false;
    const float g = 3.3f; //gravity sim
    float jumpVel = 70.f;

    // increase speed over time
    float gameSpeed = 1.f;
    float speedIncrement = 0.2f;
    const float SPEED_UP_DELAY = 5000.f;
    float next_speedup = 5000.f;

    // tutorial
    float curr_instruction_timer = 4000.f;
    const float INSTRUCTION_TIMER = 4000.f; // how long instructions will stay on screen
    bool isShowingInstructions = true;
    int tempUIZIncrement = 200;

    // for buddy stats
    const float MAX_TIREDNESS_REDUCTION = 200.f; //for demo, but rlly 300?
    const float MAX_HUNGER_ADDITION = 200.f * 4.f;

};

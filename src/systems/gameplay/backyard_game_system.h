#pragma once

#include <random>
#include "systems/core/world_system.hpp"

class BackyardGameSystem {

public:
    BackyardGameSystem(RenderSystem* renderer) : renderer(renderer) {
        
    }
    
    void step(float elapsed_ms);
    
    void beginGame();

    void createEnemyAI(float angle);

    void displayEndMsg(bool isWin);

    void displayUI(bool isDisplay);

    void endGame();

    void setupBackyardGame(Room* gameRoom);

    bool isGameRunning() const {
        return isRunning;
    }

    void playerInput(vec2 mouseMove);

    void updateStatusBars();

    void takeDamage(Entity entity);

    // void enableShoot();

    void shoot();

    bool lastBullet = false;
    bool noBullet = false;

    Entity ammo1;
    Entity ammo2;

    void incrementScore();

    void stopInstructions();

    vec2 roomC = vec2(0.f, 0.f);
    vec2 roomS = vec2(0.f, 0.f);
    vec2 roomMin = vec2(0.f, 0.f);
    vec2 roomMax = vec2(0.f, 0.f);

    int backyardLevel = 1;

    WorldSystem* worldSystem = nullptr;

private:
    RenderSystem* renderer;
    CollisionHandler handler;
    Room* room;

    bool isRunning = false;

    float score = 0.f;
    float timer = 60.f;

    // Instructions
    bool isShowingInstructions;
    float INSTRUCTION_TIME = 10.f;
    float INSTRUCTION_CUTOFF = timer - INSTRUCTION_TIME;

    // UI
    int tempUIZIncrement = 200;

    // Gun mechanics
    Entity gun;
    const size_t MAX_PROJECTILES = 2;
    bool isClick;
    vec2 mousePosition;

    // text entities
    Entity displayText;
    Entity scoreEntity;
    Entity timerEntity;
    Entity instructionEntity;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist;

};

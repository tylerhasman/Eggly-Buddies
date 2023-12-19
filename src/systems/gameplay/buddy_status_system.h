#pragma once

#include <random>
#include "systems/core/world_system.hpp"

enum class Personality {

    Baby,
    Smart,
    Cute,
    Jock,
    Cool,
    Golden,
    Hobo,

    Count
};

enum class BuddyActionType {
    Dead,
    Idle,
    Jump,
    Run,
    Sleep
};

enum class BuddyGrowthStage {
    Baby,
    Teen,
    Adult
};

class BuddyStatusSystem {

public:
    BuddyStatusSystem(RenderSystem* renderer, WorldSystem* world) : 
        renderer(renderer),
        worldSystem(world) {

        initBuddyTextures();
    }

    void step(float elapsed_ms);
    void updateEgglyStatus(float elapsed_ms);
    void updateBar(float elapsed_ms);

    void getBuddyLook(BuddyActionType actionType, Textures& texture, SpriteSheet& spriteSheet);

    vec2 getBuddySize() const;

    // float dirtiness = 0.f;
    // float hunger = 0.f;
    // float tiredness = 0.f;
    
    float entertainment = 100.f;
    float hygiene = 100.f;
    float rest = 100.f;
    float satiation = 100.f;

    const float MAX_STAT = 100.f;
    const float MIN_STAT = 0.f;
    // In seconds
    float age = 0.0f;

    Personality personality = Personality::Baby;
    BuddyGrowthStage growthStage = BuddyGrowthStage::Baby;

    bool isReadyToEvolve();

    void evolveBuddy();

    bool isDead() const;

private:
    RenderSystem* renderer;
    WorldSystem* worldSystem;

    std::unordered_map<Personality, std::unordered_map<BuddyGrowthStage, std::unordered_map<BuddyActionType, Textures>>> buddyTextures;

    void initBuddyTextures();

    float teenGrowthAge = 120.0f;
    float adultGrowthAge = 300.0f;
};

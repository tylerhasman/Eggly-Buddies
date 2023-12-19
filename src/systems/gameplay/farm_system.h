#pragma once

#include "systems/core/world_system.hpp"

class FarmSystem {

public:

    FarmSystem(RenderSystem* renderer, WorldSystem* world) : renderer(renderer), worldSystem(world) {

    }

    void step(float elapsed_ms);
    void onGrownClicked(Entity entity);
    // void startGrowth(Entity entity);

private:
    RenderSystem* renderer;
    WorldSystem* worldSystem;
};

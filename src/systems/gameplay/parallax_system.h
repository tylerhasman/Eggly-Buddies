//
// Created by Tyler on 2023-10-19.
//

#pragma once

#include "systems/core/render_system.hpp"

class ParallaxSystem {

public:
    ParallaxSystem(RenderSystem* renderSystem) : renderer(renderSystem) {

    }

    void step(float elapsed_ms);

private:
    RenderSystem* renderer;

    void updateScrolling(float elapsed_ms);
};

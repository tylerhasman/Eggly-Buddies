//
// Created by Tyler on 2023-11-19.
//

#include "ai_system.h"
#include "engine/tiny_ecs_registry.hpp"

void AISystem::step(float elapsed_ms) {
    for(auto& controller : registry.aiControllers.components){
        controller.stateMachine.step(elapsed_ms);
    }
}

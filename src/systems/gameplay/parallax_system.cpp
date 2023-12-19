//
// Created by Tyler on 2023-10-19.
//

#include "parallax_system.h"
#include "engine/tiny_ecs_registry.hpp"

void ParallaxSystem::step(float elapsed_ms) {
    updateScrolling(elapsed_ms);
}

// change it to be using shader 
void ParallaxSystem::updateScrolling(float elapsed_ms) {
    for(Entity entity : registry.scrolling.entities){

        Scrolling& scrolling = registry.scrolling.get(entity);

        scrolling.alpha = std::min(1.0f, scrolling.alpha + elapsed_ms / 1000.0f * scrolling.speed);

        if(scrolling.alpha >= 1.0){
            scrolling.alpha = 0.0;
        }

        if(registry.motions.has(entity)){
            Motion& motion = registry.motions.get(entity);

            //Linearly interpolate
            motion.position = scrolling.startPosition + (scrolling.endPosition - scrolling.startPosition) * scrolling.alpha;
        }

    }
}
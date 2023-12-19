//
// Created by Tyler on 2023-10-19.
//

#include "pickup_system.h"
#include "engine/tiny_ecs_registry.hpp"

void PickupSystem::step(float elapsed_ms) {
    for(Entity& entity : registry.pickupables.entities){
        if(registry.motions.has(entity)){
            Motion& motion = registry.motions.get(entity);
            Pickupable& pickupable = registry.pickupables.get(entity);

            if(pickupable.pickedUp){
                motion.position = world->mouseWorldPosition;
                motion.velocity = vec2(0, 0);
            }
        }
    }
}

void PickupSystem::onPickupClicked(Entity entity) {

    Entity* currentlyPickedUp = getCurrentlyPickedUp();

    if(currentlyPickedUp){
        registry.pickupables.get(*currentlyPickedUp).pickedUp = false;
    }else{
        Pickupable& pickupable = registry.pickupables.get(entity);

        pickupable.pickedUp = true;
    }

}

Entity* PickupSystem::getCurrentlyPickedUp() {

    for(Entity& entity : registry.pickupables.entities){
        Pickupable& pickupable = registry.pickupables.get(entity);

        if(pickupable.pickedUp){
            return &entity;
        }
    }

    return nullptr;
}

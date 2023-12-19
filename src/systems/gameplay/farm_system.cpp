#include "farm_system.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/pickup_system.h"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"

void FarmSystem::step(float elapsed_ms) {
    for(Entity& entity : registry.growables.entities){
        Growable& growable = registry.growables.get(entity);
        if(!growable.grown) {
            growable.remainingGrowthTime -= elapsed_ms / 1000.0f;
            if (growable.remainingGrowthTime <= 0) {
                growable.grown = true;
                Motion motion = registry.motions.get(entity);
                Entity particles = create_particles(renderer, vec2(motion.position), vec2(16, 16), Textures::STAR, -3);
                growable.particles = particles;
            }
        }
    }
}

// void FarmSystem::onGrownClicked(Entity entity) {
//     registry.growables.remove(entity);
//     // Backyard::spawnItem((world->mousePosition));
//     Backyard::spawnItem(vec2(250,250), vec2(64, 64), Textures::APPLE);


// }

// void FarmSystem::startGrowth(Growable growable, float growthTime) {

// }

// void PickupSystem::onPickupClicked(Entity entity) {

//     Entity* currentlyPickedUp = getCurrentlyPickedUp();

//     if(currentlyPickedUp){
//         registry.pickupables.get(*currentlyPickedUp).pickedUp = false;
//     }else{
//         Pickupable& pickupable = registry.pickupables.get(entity);

//         pickupable.pickedUp = true;
//     }

// }

// Entity* PickupSystem::getCurrentlyPickedUp() {

//     for(Entity& entity : registry.pickupables.entities){
//         Pickupable& pickupable = registry.pickupables.get(entity);

//         if(pickupable.pickedUp){
//             return &entity;
//         }
//     }

//     return nullptr;
// }

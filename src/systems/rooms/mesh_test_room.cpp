//
// Created by Tyler on 2023-10-31.
//

#include "mesh_test_room.h"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"

void MeshTestRoom::setupRoom(WorldSystem *world) {
    Entity meshTest = createSalmon(renderer, roomMin + roomCenter);

    CollisionHandler handler;
    handler.onCollision = [&](Entity e1, Collision collision) {
        if(registry.pickupables.has(collision.other_entity)){
            Motion& pickupMotion = registry.motions.get(collision.other_entity);

            pickupMotion.position = pickupMotion.lastPhysicsPosition;

            vec2 reflectedVelocity = pickupMotion.velocity - 2.0f * glm::dot(pickupMotion.velocity, collision.normal) * collision.normal;

            // Apply the reflected velocity
            pickupMotion.velocity = glm::normalize(reflectedVelocity);
        }
    };
    registry.collisionHandlers.emplace(meshTest, handler);

    registry.motions.get(meshTest).enableMeshCollision = true;
}

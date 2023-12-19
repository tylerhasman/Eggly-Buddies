//
// Created by Tyler on 2023-10-19.
//

#include "room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/pickup_system.h"

Entity Room::spawnEntity(vec2 position, vec2 size, Textures texture, int z) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.position = position + roomMin;

    motion.scale = size;

    Boundary boundary;
    boundary.minBoundary = roomMin + vec2(40.0f, 20.0f);
    boundary.maxBoundary = roomMax - vec2(40.0f, 20.0f);

    registry.boundaries.emplace(entity, boundary);

    RenderRequest request = {texture,
                             Effects::TEXTURED,
                             GEOMETRY_BUFFER_ID::SPRITE,
                             z
    };

    registry.renderRequests.insert(
            entity,
            request);

    return entity;
}

Entity Room::spawnItem(vec2 position, vec2 size, Textures texture, int z) {
    Entity ent = spawnEntity(position, size, texture, z);

    const Pickupable pickupable;

    Clickable clickable;
    clickable.onClick = PickupSystem::onPickupClicked;

    registry.pickupables.emplace(ent, pickupable);

    registry.clickables.emplace(ent, clickable);

    registry.boundaries.get(ent).minBoundary.y -= 50.0f;

    return ent;
}

Entity Room::spawnParticle(vec2 position, vec2 size, Textures texture, int z) {
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.position = position + roomMin;

    motion.scale = size;

    Boundary boundary;
    boundary.minBoundary = roomMin + vec2(40.0f, 20.0f);
    boundary.maxBoundary = roomMax - vec2(40.0f, 20.0f);

    registry.boundaries.emplace(entity, boundary);

    RenderRequest request = {texture,
                             Effects::PARTICLES,
                             GEOMETRY_BUFFER_ID::SPRITE,
                             z
    };

    registry.renderRequests.insert(
            entity,
            request);

    return entity;
}

//
// Created by Tyler on 2023-10-19.
//

#pragma once

#include "engine/common.hpp"
#include "engine/components.hpp"
#include "systems/core/render_system.hpp"

class WorldSystem;

class Room {
public:

    Room(vec2 min, vec2 max, RenderSystem *renderSystem) : roomMin(min), roomMax(max), renderer(renderSystem) {
        roomSize = glm::abs(roomMax - roomMin);

        // Technically this was supposed to be the middle of the room (i.e. the mid point between roomMin and roomMax)
        // However, I overlooked how it was being used in spawnEntity to spawn centered entities in the room
        // So I've opted to change it to the half-size of the rooms width/height so that it doesn't break how
        // its being used now...
        // - Tyler
        roomCenter = roomSize / 2.0f;
    }

    virtual ~Room(){

    }

    /**
     * Setup this room's entities.
     *
     * Use spawnEntity to do this as it will nicely make sure
     * entities are inside the room wherever it may be in the layout.
     */
    virtual void setupRoom(WorldSystem* world) = 0;

    Entity spawnEntity(vec2 position, vec2 size, Textures texture, int z = 0);

    Entity spawnItem(vec2 position, vec2 size, Textures texture, int z = -1);

    Entity spawnParticle(vec2 position, vec2 size, Textures texture, int z = -3);

    vec2 roomCenter;
    vec2 roomSize;
    vec2 roomMin, roomMax;


private:
    RenderSystem* renderer;
};

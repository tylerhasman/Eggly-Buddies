//
// Created by Tyler on 2023-10-31.
//

#pragma once

#include "room.h"

class MeshTestRoom : public Room {

public:

    MeshTestRoom(vec2 min, vec2 max, RenderSystem *renderSystem) : Room(min, max, renderSystem) {
        renderer = renderSystem;
    }

    void setupRoom(WorldSystem *world) override;

private:
    RenderSystem* renderer;

};

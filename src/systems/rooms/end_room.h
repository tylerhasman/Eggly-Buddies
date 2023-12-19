#pragma once

#include "room.h"

class EndRoom : public Room {
public:
    EndRoom(vec2 min, vec2 max, RenderSystem* renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
};
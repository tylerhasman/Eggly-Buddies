#pragma once

#include "room.h"

class DreamGameRoom : public Room {
public:
    DreamGameRoom(vec2 min, vec2 max, RenderSystem* renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
};
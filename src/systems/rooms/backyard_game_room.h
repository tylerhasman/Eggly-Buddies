#pragma once

#include "room.h"

#include <iostream>

class BackyardGameRoom : public Room {
public:
    BackyardGameRoom(vec2 min, vec2 max, RenderSystem* renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
};

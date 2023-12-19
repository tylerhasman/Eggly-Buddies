#pragma once

#include "room.h"

#include <iostream>

class KitchenGameRoom : public Room {
public:
    KitchenGameRoom(vec2 min, vec2 max, RenderSystem* renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
};

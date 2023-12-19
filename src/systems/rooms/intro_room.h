#pragma once

#include "room.h"

class IntroRoom : public Room {
public:
    IntroRoom(vec2 min, vec2 max, RenderSystem *renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
};

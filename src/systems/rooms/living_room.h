//
// Created by Tyler on 2023-10-19.
//

#pragma once

#include "room.h"

class LivingRoom : public Room {
public:
    LivingRoom(vec2 min, vec2 max, RenderSystem *renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;

    Entity tutorialLeftKey, tutorialRightKey;

};

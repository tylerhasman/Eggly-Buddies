//
// Created by Tyler on 2023-10-19.
//

#pragma once

#include "systems/core/world_system.hpp"

class PickupSystem {

public:

    PickupSystem(WorldSystem* world) : world(world){

    }

    void step(float elapsed_ms);

    static void onPickupClicked(Entity entity);

    static Entity* getCurrentlyPickedUp();

private:
    WorldSystem* world;

};

#include "backyard_game_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <vector>

void BackyardGameRoom::setupRoom(WorldSystem* world) {
    spawnEntity(roomCenter, roomSize, Textures::BACKYARD_GAME_BACKGROUND, 1);

    backyardGameSystem->roomC = roomCenter;
    backyardGameSystem->roomS = roomSize;
    backyardGameSystem->roomMin = roomMin;

    // vec2 plateCoords = roomCenter + roomSize.y / 2.0f - 40.0f;

    // std::cout << plateCoords.x << " " << plateCoords.y << std::endl;

    // Entity plate = spawnEntity(plateCoords, vec2(150, 37.5), Textures::PLATE, -1);

    // // set constraints on plate's x and y so that it can't go past the edges of the screen


    backyardGameSystem->setupBackyardGame(this);

}

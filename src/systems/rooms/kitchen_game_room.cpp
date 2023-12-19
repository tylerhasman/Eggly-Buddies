#include "kitchen_game_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <vector>

void KitchenGameRoom::setupRoom(WorldSystem* world) {

    spawnEntity(roomCenter, roomSize, Textures::DAWNSKY1, 4);
    Entity dawnSky2 = spawnEntity(roomCenter, roomSize, Textures::DAWNSKY2, 3);
    Entity dawnSky3 = spawnEntity(roomCenter, roomSize, Textures::DAWNSKY3, 2);
    Entity dawnSky4 = spawnEntity(roomCenter, roomSize, Textures::DAWNSKY4, 1);

    RenderRequest& dawnSky2Req = registry.renderRequests.get(dawnSky2);
    dawnSky2Req.iUniforms["u_EnableParallax"] = 1;
    dawnSky2Req.fUniforms["u_ParallaxSpeed"] = 0.1f;
    RenderRequest& dawnSky3Req = registry.renderRequests.get(dawnSky3);
    dawnSky3Req.iUniforms["u_EnableParallax"] = 1;
    dawnSky3Req.fUniforms["u_ParallaxSpeed"] = 0.15f;
    RenderRequest& dawnSky4Req = registry.renderRequests.get(dawnSky4);
    dawnSky4Req.iUniforms["u_EnableParallax"] = 1;
    dawnSky4Req.fUniforms["u_ParallaxSpeed"] = 0.2f;

    vec2 plateCoords = roomCenter + roomSize.y / 2.0f - 40.0f;

    std::cout << plateCoords.x << " " << plateCoords.y << std::endl;

    Entity plate = spawnEntity(plateCoords, vec2(150, 37.5), Textures::PLATE, -2);

    // set constraints on plate's x and y so that it can't go past the edges of the screen


    kitchenGameSystem->setupKitchenGame(this, plate);

}

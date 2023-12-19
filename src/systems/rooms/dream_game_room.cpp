#include "dream_game_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <vector>

void DreamGameRoom::setupRoom(WorldSystem* world) {
    std::cout << "Dream Room roomC.x: " << "roomCenter" << std::endl;
    dreamGameSystem->roomC = roomCenter;
    dreamGameSystem->roomS = roomSize;
    dreamGameSystem->roomMin = roomMin;
    spawnEntity(roomCenter, roomSize, Textures::TWISKY1, 10);
    Entity twiSky2 = spawnEntity(roomCenter, roomSize, Textures::TWISKY2, 9);
    Entity twiSky3 = spawnEntity(roomCenter, roomSize, Textures::TWISKY3, 8);
    Entity twiSky4 = spawnEntity(roomCenter, roomSize, Textures::TWISKY4, 7);
    Entity twiSky5 = spawnEntity(roomCenter, roomSize, Textures::TWISKY5, 6);

    Entity mountain1 = spawnEntity(roomCenter, roomSize, Textures::MOUNTAIN1, 5);
    Entity mountain2 = spawnEntity(roomCenter, roomSize, Textures::MOUNTAIN2, 4);
    Entity mountain3 = spawnEntity(roomCenter, roomSize, Textures::MOUNTAIN3, 3);

    Entity ground = spawnEntity(roomCenter, roomSize, Textures::DREAM_GAME_GROUND, 0);

    RenderRequest& twiSky2Req = registry.renderRequests.get(twiSky2);
    twiSky2Req.iUniforms["u_EnableParallax"] = 1;
    twiSky2Req.fUniforms["u_ParallaxSpeed"] = 0.1f;
    RenderRequest& twiSky3Req = registry.renderRequests.get(twiSky3);
    twiSky3Req.iUniforms["u_EnableParallax"] = 1;
    twiSky3Req.fUniforms["u_ParallaxSpeed"] = 0.15f;
    RenderRequest& twiSky4Req = registry.renderRequests.get(twiSky4);
    twiSky4Req.iUniforms["u_EnableParallax"] = 1;
    twiSky4Req.fUniforms["u_ParallaxSpeed"] = 0.2f;
    RenderRequest& twiSky5Req = registry.renderRequests.get(twiSky5);
    twiSky5Req.iUniforms["u_EnableParallax"] = 1;
    twiSky5Req.fUniforms["u_ParallaxSpeed"] = 0.25f;

    RenderRequest& mountain1Req = registry.renderRequests.get(mountain1);
    mountain1Req.iUniforms["u_EnableParallax"] = 1;
    mountain1Req.fUniforms["u_ParallaxSpeed"] = 0.3f;
    RenderRequest& mountain2Req = registry.renderRequests.get(mountain2);
    mountain2Req.iUniforms["u_EnableParallax"] = 1;
    mountain2Req.fUniforms["u_ParallaxSpeed"] = 0.5f;
    RenderRequest& mountain3Req = registry.renderRequests.get(mountain3);
    mountain3Req.iUniforms["u_EnableParallax"] = 1;
    mountain3Req.fUniforms["u_ParallaxSpeed"] = 0.7f;

    RenderRequest& groundRequest = registry.renderRequests.get(ground);
    groundRequest.iUniforms["u_EnableParallax"] = 1;
    groundRequest.fUniforms["u_ParallaxSpeed"] = 4.f;

}

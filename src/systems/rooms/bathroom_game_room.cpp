#include "bathroom_game_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <vector>


void BathroomGameRoom::setupRoom(WorldSystem* world) {

    spawnEntity(roomCenter, roomSize, Textures::BATHROOM_GAME_BACKGROUND, 2);
    Entity cloud = spawnEntity(roomCenter, roomSize, Textures::BATHROMM_GAME_CLOUD, 1);

    RenderRequest& cloudReq = registry.renderRequests.get(cloud);
    cloudReq.iUniforms["u_EnableParallax"] = 1;
    cloudReq.fUniforms["u_ParallaxSpeed"] = 0.1f;

    bathroomGameSystem->setUpSystem(this);

}
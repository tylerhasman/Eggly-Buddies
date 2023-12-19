#include "intro_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"

void IntroRoom::setupRoom(WorldSystem* world) {
    spawnEntity(roomCenter, roomSize, Textures::TUTSKY1, 2);
    Entity sky = spawnEntity(roomCenter, roomSize, Textures::TUTSKY2, 1);
    spawnEntity(roomCenter, roomSize, Textures::TUTBACKGROUND, 0);

    RenderRequest& skyReq = registry.renderRequests.get(sky);
    skyReq.iUniforms["u_EnableParallax"] = 1;
    skyReq.fUniforms["u_ParallaxSpeed"] = 0.4f;
}

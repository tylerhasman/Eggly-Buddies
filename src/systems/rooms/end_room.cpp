#include "end_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"
Entity buddy;
void EndRoom::setupRoom(WorldSystem* world) {
    endSystem->roomC = roomCenter;
    spawnEntity(roomCenter, roomSize, Textures::TUTSKY1, 2);
    Entity sky = spawnEntity(roomCenter, roomSize, Textures::TUTSKY2, 1);
    spawnEntity(roomCenter, roomSize, Textures::TUTBACKGROUND, 0);

    vec2 buddySize = buddyStatusSystem->getBuddySize();
    buddy = spawnEntity(vec2(roomCenter.x, roomCenter.y + 220), buddySize, Textures::EGG_IDLE, -100);
    SpriteSheet spriteSheet;
    spriteSheet.dimensions = vec2(2, 1);
    spriteSheet.current = vec2(0, 0);
    spriteSheet.frameSpeed = 0.5f;
    registry.spriteSheets.emplace(buddy, spriteSheet);
    buddyStatusSystem->getBuddyLook(BuddyActionType::Idle, //wtf
        registry.renderRequests.get(buddy).used_texture, spriteSheet);

    RenderRequest& skyReq = registry.renderRequests.get(sky);
    skyReq.iUniforms["u_EnableParallax"] = 1;
    skyReq.fUniforms["u_ParallaxSpeed"] = 0.4f;
}
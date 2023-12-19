//
// Created by Tyler on 2023-10-19.
//

#include "living_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"

void LivingRoom::setupRoom(WorldSystem* world) {

    spawnEntity(roomCenter, roomSize, Textures::LIVING_ROOM, 0);

    spawnEntity(roomCenter, roomSize, Textures::BLUESKY1, 5);
    Entity bluesky2 = spawnEntity(roomCenter, roomSize, Textures::BLUESKY2, 4);
    Entity bluesky3 = spawnEntity(roomCenter, roomSize, Textures::BLUESKY3, 3);
    Entity bluesky4 = spawnEntity(roomCenter, roomSize, Textures::BLUESKY4, 2);

    RenderRequest& sky1 = registry.renderRequests.get(bluesky2);
    sky1.iUniforms["u_EnableParallax"] = 1;
    sky1.fUniforms["u_ParallaxSpeed"] = 0.25f;
    RenderRequest& sky2 = registry.renderRequests.get(bluesky3);
    sky2.iUniforms["u_EnableParallax"] = 1;
    sky2.fUniforms["u_ParallaxSpeed"] = 0.5f;
    RenderRequest& sky3 = registry.renderRequests.get(bluesky4);
    sky3.iUniforms["u_EnableParallax"] = 1;
    sky3.fUniforms["u_ParallaxSpeed"] = 0.75f;

    tutorialLeftKey = spawnEntity(roomCenter - vec2(100.0f, 0.0f) + vec2(0, 150), vec2(40, 42) * 1.5f, Textures::LEFT_KEY, -5);
    tutorialRightKey = spawnEntity(roomCenter + vec2(100.0f, 0.0f) + vec2(0, 150), vec2(40, 42) * 1.5f, Textures::RIGHT_KEY, -5);

    SpriteSheet& leftKeySpriteSheet = registry.spriteSheets.emplace(tutorialLeftKey);
    leftKeySpriteSheet.dimensions = vec2(2, 1);
    leftKeySpriteSheet.frameSpeed = 0.5f;
    leftKeySpriteSheet.looping = true;

    SpriteSheet& rightKeySpriteSheet = registry.spriteSheets.emplace(tutorialRightKey);
    rightKeySpriteSheet.dimensions = vec2(2, 1);
    rightKeySpriteSheet.frameSpeed = 0.5f;
    rightKeySpriteSheet.looping = true;

    /*for (int i = 0; i < 5; i++) {
        Textures texture = (i > 3) ? Textures::LARGE_CLOUD : Textures::SMALL_CLOUD;

        const float size = world->random() * 75.0f + 85.0f;

        Entity cloud = spawnEntity(vec2(0, 200) + roomMin, vec2(size, size), texture, 4);

        const float randomY = (world->random() * 250.0f - 250.0f / 2.0f);

        Scrolling scrolling = {
                vec2(0.0f, 250.0f + randomY),
                vec2(game_width, 250.0f + randomY),
                0.1f + world->random() * 0.1f,
                world->random()
        };

        registry.scrolling.emplace(cloud, scrolling);
    }*/

}

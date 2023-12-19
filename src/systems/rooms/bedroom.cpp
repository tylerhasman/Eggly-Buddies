#include "bedroom.h"
#include "engine/tiny_ecs.hpp"
#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "engine/world_init.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/core/world_system.hpp"
#include "systems/ai/buddy_state_machine.h"
#include <iostream>

bool isSleep = false;
vec2 roomC;
vec2 roomS;
Bedroom* bedroom;

Entity dirtyText;
void Bedroom::setupRoom(WorldSystem* world) {
    bedroom = this;
    roomC = roomCenter;
    roomS = roomSize;
    isSleep = false;
    worldSystem = world;
    dreamGameSystem->worldSystem = world;
    player = registry.players.entities[0];
    spawnEntity(roomCenter, roomSize, Textures::BEDROOM, 1);
    Entity lightSwitch = spawnEntity(vec2(roomCenter.x - 20.f, roomCenter.y - 20.f), vec2(roomSize.x/10.f, roomSize.y/6.f), Textures::LAMP, 0);
    Clickable clickable;

    clickable.onClick = [this](auto entity){
        this->onLightSwitched(entity);
    };

    registry.clickables.emplace(lightSwitch, clickable);

}

void Bedroom::onLightSwitched(Entity entity) {
    std::cout << "Lightswitch hit" << std::endl;
    if (buddyStatusSystem->hygiene < MIN_HYGIENE && !isSleep) { //buddy is stinky poopy nasty dont let him in bed
        std::cout << "NUH UH" << std::endl;
        chatboxSystem->createBox("Buddy is too dirty to get into bed!");
        return;
    }
    if (isSleep) { //is sleeping, trigger awake state
        isSleep = false;

        Player& p = registry.players.get(player);
        p.enableGravity = true;

        AiController& controller = registry.aiControllers.get(worldSystem->getBuddy());
        controller.stateMachine.enterState(BuddyState::States::Idle);

        SpriteSheet& spriteSheet = registry.spriteSheets.get(player);
        spriteSheet.dimensions = vec2(1, 1);
        spriteSheet.current = vec2(0, 0);
        spriteSheet.frameSpeed = 0.f;
        RenderRequest& guyRender = registry.renderRequests.get(player);
        guyRender.used_texture = Textures::TEST_GUY;
        Motion& playerMotion = registry.motions.get(registry.players.entities[0]);

        playerMotion.position = roomMin + vec2(roomS.x / 2.0f, roomS.y / 2.0f + 286.0f);

        for (RenderRequest& renderReq : registry.renderRequests.components) { //turn on lights
            renderReq.iUniforms["u_EnableDarken"] = 0;
        }
        for (Entity other : registry.sleepStates.entities) { //delete bubble
            registry.sleepStates.remove(other);
            registry.remove_all_components_of(other);
        }
    }
    else
    {
        isSleep = true;

        AiController& controller = registry.aiControllers.get(worldSystem->getBuddy());
        controller.stateMachine.enterState(BuddyState::States::NoAi);

        for (RenderRequest& renderReq : registry.renderRequests.components) { //turn off lights
            renderReq.iUniforms["u_EnableDarken"] = 1;
            renderReq.fUniforms["u_DarkenFloat"] = 0.6f;
        }

        Entity dreamBubble = bedroom->spawnEntity(vec2(roomC.x * 5.f / 8.f, roomC.y * 3.f / 3.2f), vec2(roomS.x / 10.f, roomS.y / 8.f), Textures::DREAM_BUBBLE, 0);
        registry.sleepStates.emplace(dreamBubble);
        Clickable clickable;
        clickable.onClick = [this](auto entity) {
            onBubbleClicked(entity);
        };
        registry.clickables.emplace(dreamBubble, clickable);
        Motion& playerMotion = registry.motions.get(registry.players.entities[0]);
        playerMotion.position = roomMin + vec2(roomS.x * 0.23f, roomS.y * 2.f / 3.5f + 95.f);
        SpriteSheet& spriteSheet = registry.spriteSheets.get(player);
        spriteSheet.dimensions = vec2(2, 1);
        spriteSheet.current = vec2(0, 0);
        spriteSheet.frameSpeed = 0.5f;
        RenderRequest& guyRender = registry.renderRequests.get(player);
        guyRender.used_texture = Textures::GUY_SLEEP_SPRITESHEET;

        Player& p = registry.players.get(player);
        p.enableGravity = false;

    }
}

void Bedroom::onBubbleClicked(Entity entity) {
    worldSystem->changeRoom(-1, -1);
    dreamGameSystem->beginGame();
}

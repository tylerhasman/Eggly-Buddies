//
// Created by Tyler on 2023-10-30.
//

#include "gameplay_systems.h"
#include "systems/ai/ai_system.h"

MailSystem* mailSystem;
ParallaxSystem* parallaxSystem;
DreamGameSystem* dreamGameSystem;
KitchenGameSystem* kitchenGameSystem;
BackyardGameSystem* backyardGameSystem;
PickupSystem* pickupSystem;
FarmSystem* farmSystem;
BuddyStatusSystem* buddyStatusSystem;
WorldSystem* worldSystem;
BathroomGameSystem* bathroomGameSystem;
ChatboxSystem* chatboxSystem;
IntroSystem* introSystem;
EndSystem* endSystem;

void GameplaySystems::initializeGameplaySystems(WorldSystem* world, RenderSystem* renderSystem) {
    worldSystem = world;
    mailSystem = new MailSystem(renderSystem);
    parallaxSystem = new ParallaxSystem(renderSystem);
    dreamGameSystem = new DreamGameSystem(renderSystem);
    kitchenGameSystem = new KitchenGameSystem(renderSystem);
    backyardGameSystem = new BackyardGameSystem(renderSystem);
    pickupSystem = new PickupSystem(world);
    farmSystem = new FarmSystem(renderSystem, world);
    buddyStatusSystem = new BuddyStatusSystem(renderSystem, world);
    bathroomGameSystem = new BathroomGameSystem(renderSystem);
    chatboxSystem = new ChatboxSystem(renderSystem);
    introSystem = new IntroSystem(renderSystem);
    endSystem = new EndSystem(renderSystem);
}

void GameplaySystems::destroyGameplaySystems() {
    delete mailSystem;
    delete parallaxSystem;
    delete dreamGameSystem;
    delete kitchenGameSystem;
    delete backyardGameSystem;
    delete pickupSystem;
    delete farmSystem;
    delete buddyStatusSystem;
    delete bathroomGameSystem;
    delete chatboxSystem;
    delete introSystem;
    delete endSystem;
}

void GameplaySystems::step(float elapsed_ms) {
    mailSystem->step(elapsed_ms);
    parallaxSystem->step(elapsed_ms);
    pickupSystem->step(elapsed_ms);
    dreamGameSystem->step(elapsed_ms);
    kitchenGameSystem->step(elapsed_ms);
    backyardGameSystem->step(elapsed_ms);
    farmSystem->step(elapsed_ms);
    buddyStatusSystem->step(elapsed_ms);
    bathroomGameSystem->step(elapsed_ms);
    AISystem::step(elapsed_ms);
}

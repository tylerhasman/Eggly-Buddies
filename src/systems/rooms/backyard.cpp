#include "backyard.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/pickup_system.h"
#include "systems/gameplay/inventory_system.h"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/ai/buddy_state_machine.h"
#include "systems/rooms/kitchen_room.h"
#include "engine/world_init.hpp"

Backyard* backyard;
Entity Backyard::basketEntity;
void Backyard::setupRoom(WorldSystem* world) {
    worldSystem = world;
    backyardGameSystem->worldSystem = world;
    backyard = this;

    spawnEntity(roomCenter, roomSize, Textures::BACKYARD, 1);
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

    spawnSeedPlot(roomCenter - vec2(448, -238));
    spawnSeedPlot(roomCenter - vec2(298, -238));
    spawnSeedPlot(roomCenter - vec2(148, -238));
    spawnSeedPlot(roomCenter - vec2(-2, -238));

    Entity treeDoor =  spawnEntity(roomCenter + vec2(380, 175), vec2(48, 64), Textures::TREE_DOOR, 0);
    Clickable doorClickable;
    doorClickable.onClick = [&](Entity entity){
        onDoorClicked(entity);
        };
    registry.clickables.emplace(treeDoor, doorClickable);

    Entity basket = spawnEntity(roomCenter + vec2(400, 300), vec2(128, 128), Textures::BASKET, -1);
    basketEntity = basket;
    registry.texts.emplace(basket);

    updateBasketText(basket);

    PickupReceiver basketReceiver;
    basketReceiver.onPickupReceived = [basket](auto pickup){
        onBasketReceivePickup(basket, pickup);
    };

    registry.pickupReceivers.emplace(basket, basketReceiver);


    fertilizer = backyard->spawnEntity(roomCenter + vec2(150, 175), vec2(57, 60), Textures::FERTILIZER_SHADOW, 0);
    Clickable fertilizerClickable;
    fertilizerClickable.onClick = [&](Entity entity){
        onFertilizerClicked(entity);
        };
    registry.clickables.emplace(fertilizer, fertilizerClickable);

    registry.texts.emplace(fertilizer);

    Inventory::adjustItemAmount(InventoryItemType::Fertilizer, 1); // REMOVE AFTER

    updateFertilizerText();
}

void Backyard::onDoorClicked(Entity entity) {
    // AiController& controller = registry.aiControllers.get(worldSystem->getBuddy());
    // controller.stateMachine.enterState(BuddyStates::NoAi);

    // Fix annoying issue where u would accidently click on the door
    if(PickupSystem::getCurrentlyPickedUp() == nullptr){
        worldSystem->changeRoom(2, -1);
        backyardGameSystem->beginGame();
    }

}

void Backyard::onFertilizerClicked(Entity entity) {
    if(PickupSystem::getCurrentlyPickedUp() == nullptr){
        if (Inventory::getItemAmount(InventoryItemType::Fertilizer) > 0) {
            Motion motion = registry.motions.get(entity);
            Entity moveableFertilizer = spawnItem(motion.position, motion.scale, Textures::FERTILIZER, -4);
            Pickupable& pickupable = registry.pickupables.get(moveableFertilizer);
            pickupable.pickedUp = true;
            Inventory::adjustItemAmount(InventoryItemType::Fertilizer, -1);
            registry.fertilizers.emplace(moveableFertilizer);
            updateFertilizerText();
        }
    }
}

void Backyard::updateFertilizerText() {
    Backyard* room = worldSystem->getRoomOfType<Backyard>();

    if (room) {
        Entity fert = worldSystem->getRoomOfType<Backyard>()->fertilizer;
        
        if (registry.texts.has(fert)) {
            Text& text = registry.texts.get(fert);

            int numFertilizer = Inventory::getItemAmount(InventoryItemType::Fertilizer);

            if(numFertilizer < 0){
                text.text = "Drag fertilizer to the plants\nto make it grow instantly!";
                text.scale = 0.35f;
                text.shadow = true;
                text.offset = vec2(-155, -100);
            }else{
                text.text = "Drag fertilizer to the plants\nto make it grow instantly!\n\n\n\n       Fertilizer: " + std::to_string(numFertilizer);
                text.scale = 0.4f;
                text.shadow = true;
                text.offset = vec2(-140, -75);
            }
        }
    }
}

void Backyard::onSeedReceiveFertilizer(Entity seed, Entity pickup) {
    if (registry.fertilizers.has(pickup)) {
        for (Entity grow_entity : registry.growables.entities) {
            Growable& growable = registry.growables.get(grow_entity);
            if (growable.seedPlot == seed && !growable.grown) {
                growable.remainingGrowthTime = 0;
                RenderRequest& renderRequest = registry.renderRequests.get(grow_entity);
                renderRequest.fUniforms["u_GrowthTime"] = 0;
                registry.remove_all_components_of(pickup);
            }
        }
    }
}


void Backyard::onBasketReceivePickup(Entity basket, Entity pickup) {
    if (registry.foods.has(pickup)) {
        registry.remove_all_components_of(pickup);
        Inventory::adjustItemAmount(InventoryItemType::Food, 1);
        updateBasketText(basket);
    }
}

void Backyard::updateBasketText(Entity basket) {
    Text& text = registry.texts.get(basket);

    int numFoods = Inventory::getItemAmount(InventoryItemType::Food);

    if(numFoods == 0){
        text.text = "Drag food into the\nbasket to collect it!";
        text.scale = 0.5f;
        text.shadow = true;
        text.offset = vec2(-135, -100);
    }else{
        text.text = "Food: " + std::to_string(numFoods);
        text.scale = 0.5f;
        text.shadow = true;
        text.offset = vec2(-50, -100);
    }

    // Update the fridge text as well
    KitchenRoom::updateFryPanText();
}

void Backyard::onSeedClicked(Entity entity) {
    if(PickupSystem::getCurrentlyPickedUp() == nullptr){
        registry.clickables.remove(entity);
        Entity grow = randomPlant(entity);

        Growable& growable = registry.growables.get(grow);
        growable.seedPlot = entity;
        RenderRequest& growRenderRequest = registry.renderRequests.get(grow);
        growRenderRequest.iUniforms["u_EnableGrowth"] = 1;
        growRenderRequest.fUniforms["u_GrowthTime"] = growable.growthTime;
        growRenderRequest.fUniforms["u_InitialHeight"] = growable.initialHeight;
        growRenderRequest.fUniforms["u_FinalHeight"] = growable.finalHeight;
        growRenderRequest.fUniforms["u_StartGrowthTime"] = (float)(glfwGetTime());     
    }
}

void Backyard::onPlantClicked(Entity entity) {
    Growable& growable = registry.growables.get(entity);
    if (growable.grown) {
        if(PickupSystem::getCurrentlyPickedUp() == nullptr){
            Clickable seedPlotClickable;
            seedPlotClickable.onClick = [&](Entity entity){
            onSeedClicked(entity);
            };
            Entity& seedPlot = growable.seedPlot;
            registry.clickables.emplace(seedPlot, seedPlotClickable);

            Entity& particles = growable.particles;
            registry.remove_all_components_of(particles);

            Motion motion = registry.motions.get(entity);
            Entity plant = spawnItem(motion.position, motion.scale, registry.renderRequests.get(entity).used_texture);
            Pickupable& pickupable = registry.pickupables.get(plant);
            pickupable.pickedUp = true;
            
            registry.foods.emplace(plant);

            registry.remove_all_components_of(entity);
        }
    }
}

Entity Backyard::spawnGrowable(vec2 position, vec2 size, Textures texture, Entity seedPlot, float growthTime, float initialHeight, float finalHeight) {
    Entity ent = spawnEntity(position - roomMin, size, texture, -1);
    auto& growable = registry.growables.emplace(ent);
    growable.remainingGrowthTime = growthTime;
    growable.growthTime = growthTime;
    growable.initialHeight = initialHeight;
    growable.finalHeight = finalHeight;
    // growable.seedPlot = seedPlot;
    Clickable clickable;
    clickable.onClick = [&](Entity entity){
        onPlantClicked(entity);

    };
    registry.clickables.emplace(ent, clickable);
    return ent;
}

Entity Backyard::spawnSeedPlot(vec2 position) {
    Entity seedPlot = spawnEntity(position, vec2(64, 36), Textures::SEED_PLOT, 0);
    Clickable clickable;
    clickable.onClick = [&](Entity entity){
        onSeedClicked(entity);
    };
    registry.clickables.emplace(seedPlot, clickable);

    PickupReceiver seedReceiver;
    seedReceiver.onPickupReceived = [seedPlot](auto pickup){
        onSeedReceiveFertilizer(seedPlot, pickup);
    };

    registry.pickupReceivers.emplace(seedPlot, seedReceiver);

    return seedPlot;
}

Entity Backyard::randomPlant(Entity entity) {
    Motion motion = registry.motions.get(entity);

    std::random_device rd;
    std::default_random_engine rng(rd());
    std::uniform_int_distribution<int> distribution(1,4);
    int randomPlant = distribution(rng);
    Entity grow;
    switch (randomPlant) {
        case 1:
            grow = spawnGrowable(motion.position + vec2(0,-32), vec2(48,48), Textures::MELON, entity, 30.0f, 0.3f, 0.7f);
            break;
        case 2:
            grow = spawnGrowable(motion.position + vec2(0,-32), vec2(48,48), Textures::CARROT, entity, 30.0f, 0.2f, 0.7f);
            break;
        case 3:
            grow = spawnGrowable(motion.position + vec2(0,-32), vec2(48, 48), Textures::PUMPKIN, entity, 30.0f, 0.2f, 0.7f);
            break;
        case 4:
            grow = spawnGrowable(motion.position + vec2(0,-32), vec2(48,48), Textures::BANANA, entity, 30.0f, 0.2f, 0.7f);
            break;
    }
    return grow;
}

// Alternate(buggy) version of picking up grown plants, could be better when we implement Interactables
        // Pickupable pickupable;
        // Pickupable& plantPickupable = registry.pickupables.emplace(entity, pickupable);
        // plantPickupable.pickedUp = true;

        // Pickupable pickupable;
        // registry.pickupables.emplace(entity, pickupable);

        // registry.clickables.remove(entity);
        // Clickable clickable;
        // clickable.onClick = PickupSystem::onPickupClicked;
        // registry.clickables.emplace(entity, clickable);
        // RenderRequest& growRenderRequest = registry.renderRequests.get(entity);
	    // growRenderRequest.iUniforms["u_EnableGrowth"] = 0;



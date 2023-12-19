//
// Created by Emily on 2023-10-29.
//

#include "kitchen_room.h"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/animation_system.h"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/gameplay/inventory_system.h"
#include "systems/rooms/backyard.h"

vec2 roomCenter;
vec2 roomSize;
KitchenRoom* kitchenRoom;
void KitchenRoom::setupRoom(WorldSystem* world) {
    kitchenRoom = this;
    roomCenter = roomCenter;
    roomSize = roomSize;
    
    spawnEntity(roomCenter, roomSize, Textures::KITCHEN_ROOM, 3);
    Entity fridge = spawnEntity(fridgePos, fridgeSize, Textures::FRIDGE, 2);


    //Entity cutting_board = spawnEntity(cuttingBoardPos, cuttingBoardSize, Textures::CUTTING_BOARD, 1);
    //Entity knife = spawnEntity(knifePos, knifeSize, Textures::KNIFE, 1);
    //registry.kitchenAppliances.emplace(cutting_board);

    //Entity apple = spawnItem(vec2(window_width_px - 50, window_height_px - 50), vec2(64, 64), Textures::APPLE);
    //registry.foods.emplace(apple);
    
    
    Clickable clickable;
    clickable.onClick = [&](Entity entity){
        onFridgeClicked(entity);
    };

    registry.kitchenEvents.emplace(fridge);
    registry.clickables.emplace(fridge, clickable);

    /*CollisionHandler handler;
    handler.onCollision = [&](Entity knife, Collision collision) {
        Entity other = collision.other_entity;
        if(registry.foods.has(other)){
            SpriteSheet apple_slicing;
            apple_slicing.dimensions = vec2(4, 1);
            apple_slicing.current = vec2(0, 0);
            apple_slicing.frameSpeed = 1.0f;
            apple_slicing.looping = false;

            sliceApple(other, apple_slicing);
        }
    };

    registry.collisionHandlers.emplace(knife, handler);*/

    frypan = kitchenRoom->spawnEntity(vec2(roomCenter.x -220, roomCenter.y -130), vec2(170*0.74, 170), Textures::PAN, 0);
    Clickable bubbleClickable;
    bubbleClickable.onClick = [this](auto entity) {
        onBubbleClicked(entity);
    };
    registry.clickables.emplace(frypan, bubbleClickable);

    registry.texts.emplace(frypan);


    updateFryPanText();
}

void KitchenRoom::onFridgeClicked(Entity entity) {
    // on clicking fridge, open or close fridge door depending on state of fridge
    Clickable fridgeClickable;
    fridgeClickable.onClick = [&](Entity entity){
        onFridgeClicked(entity);
    };

    if (fridgeOpen) {
        // close fridge door
        Entity closed_fridge = spawnEntity(fridgePos, fridgeSize, Textures::FRIDGE, 1);
        registry.kitchenEvents.remove(entity);
        registry.remove_all_components_of(entity);
        registry.kitchenEvents.emplace(closed_fridge);
        registry.clickables.emplace(closed_fridge, fridgeClickable);
        fridgeOpen = false;
    } else {
        // open fridge door
        Entity open_fridge = spawnEntity(fridgeOpenPos, fridgeOpenSize, Textures::FRIDGE_OPEN, 1);
        registry.kitchenEvents.remove(entity);
        registry.remove_all_components_of(entity);
        registry.kitchenEvents.emplace(open_fridge);
        registry.clickables.emplace(open_fridge, fridgeClickable);
        fridgeOpen = true;

    }
}

/*void KitchenRoom::sliceApple(Entity apple, SpriteSheet apple_slicing) {
    registry.foods.remove(apple);
    registry.remove_all_components_of(apple);
    
    Entity sliced_apple = spawnEntity(vec2(540,455), vec2(64, 64), Textures::APPLE_SLICING, 1);

    registry.spriteSheets.emplace(sliced_apple, apple_slicing);
}*/

void KitchenRoom::onBubbleClicked(Entity entity) {
    if(Inventory::getItemAmount(InventoryItemType::Food) >= 4){
        Inventory::adjustItemAmount(InventoryItemType::Food, -4);

        worldSystem->changeRoom(1, -1);
        kitchenGameSystem->beginGame();

        Backyard::updateBasketText(Backyard::basketEntity);
    }
}

void KitchenRoom::updateFryPanText() {

    KitchenRoom* room = worldSystem->getRoomOfType<KitchenRoom>();

    if(room){
        Entity fridge = worldSystem->getRoomOfType<KitchenRoom>()->frypan;

        if(registry.texts.has(fridge)){
            Text& text = registry.texts.get(fridge);

            int numFoods = Inventory::getItemAmount(InventoryItemType::Food);
            text.offset = vec2(-175, -100);

            if(numFoods == 0){
                text.text = "Grow food in the backyard\nto cook with it!";
                text.scale = 0.5f;
                text.shadow = true;
                text.color = vec3(1.0, 1.0, 1.0);
            }else if(numFoods < 4){
                text.text = std::to_string(4 - numFoods) +" more food needed to cook";
                text.scale = 0.5f;
                text.shadow = true;
                text.color = vec3(1.0, 1.0, 1.0);
            }else{
                text.text = "Cooking Ready!";
                text.scale = 0.5f;
                text.shadow = true;
                text.color = vec3(0.0, 1.0, 0.0);
            }
        }
    }

}

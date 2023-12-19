//
// Created by Emily on 2023-10-29.
//

#pragma once

#include "room.h"

#include <iostream>

class KitchenRoom : public Room {
public:
    KitchenRoom(vec2 min, vec2 max, RenderSystem *renderSystem) : Room(min, max, renderSystem) {
        std::cout << "KitchenRoom created" << std::endl;
    }
    
    void setupRoom(WorldSystem* world) override;
    void onFridgeClicked(Entity entity);    
    //void sliceApple(Entity apple, SpriteSheet apple_slicing);
    void onBubbleClicked(Entity entity);

    static void updateFryPanText();

protected:
    vec2 fridgePos = vec2(905,420);
    vec2 fridgeOpenPos = vec2(900+108,420);
    vec2 fridgeSize = vec2(200, 530);
    vec2 fridgeOpenSize = vec2(202*2, 530);
    vec2 cuttingBoardPos = vec2(540,480);
    vec2 cuttingBoardSize = vec2(145, 20);
    vec2 knifePos = vec2(540, 440);
    vec2 knifeSize = vec2(75, 75);

    bool fridgeOpen = false;
    int fridgeItems[10];

    Entity frypan;

    RenderSystem* renderer;

    Entity player2;
};

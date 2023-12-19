#pragma once

#include "room.h"

class Backyard : public Room {
public:
    Backyard(vec2 min, vec2 max, RenderSystem *renderSystem) : Room(min, max, renderSystem) {

    }
    
    void setupRoom(WorldSystem* world) override;
    void onSeedClicked(Entity entity);
    void onPlantClicked(Entity entity);
    void onDoorClicked(Entity entity);
    void onFertilizerClicked(Entity entity);
    Entity spawnGrowable(vec2 position, vec2 size, Textures texture, Entity seedPlot, float growthTime, float initialHeight, float finalHeight);
    Entity spawnSeedPlot(vec2 position);
    Entity randomPlant(Entity entity);
    static Entity basketEntity;
    static void updateBasketText(Entity basket);
    static void updateFertilizerText();

    static void onBasketReceivePickup(Entity basket, Entity pickup);
    static void onSeedReceiveFertilizer(Entity seed, Entity pickup);

protected:
    Entity fertilizer;
};

#pragma once

#include "room.h"

class BathRoom : public Room {
public:
    BathRoom(vec2 min, vec2 max, RenderSystem *renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
    void setPoopEntity(Entity entity) { poop= entity; poopExists = true; }
    void clearPoopEntity();
    void cleanEvent();
    void triggerBathroomGame(Entity entity);
    bool getPoopExists() const { return poopExists; }
private:
    bool poopExists = false;
    Entity poop;
};

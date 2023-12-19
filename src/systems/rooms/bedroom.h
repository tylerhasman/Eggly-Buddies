#pragma once

#include "room.h"

class Bedroom : public Room {
public:
    Bedroom(vec2 min, vec2 max, RenderSystem* renderSystem) : Room(min, max, renderSystem) {

    }

    void setupRoom(WorldSystem* world) override;
    void onLightSwitched(Entity entity);
    void onBubbleClicked(Entity entity);
    WorldSystem* worldSystem = nullptr;

private:
    Entity player;
    const float MIN_HYGIENE = 50.f;
};

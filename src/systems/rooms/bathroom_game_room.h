#pragma once

#include "room.h"

class BathroomGameRoom : public Room {
public:
    BathroomGameRoom(vec2 min, vec2 max, RenderSystem* renderSystem) : Room(min, max, renderSystem) {}

    void setupRoom(WorldSystem* world) override;
    
private:
    
    
};


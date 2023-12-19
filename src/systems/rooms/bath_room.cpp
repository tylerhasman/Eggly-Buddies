#include "bath_room.h"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/core/world_system.hpp"
#include "systems/gameplay/gameplay_systems.h"


void BathRoom::setupRoom(WorldSystem* world) {

    spawnEntity(roomCenter, roomSize, Textures::BATH_ROOM, 0);
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

    // Entity shampoo = spawnItem(vec2{roomCenter[0]+180,roomCenter[1]}, vec2(40, 60), Textures::SHAMPOO);
    // Entity bodywash = spawnItem(vec2{roomCenter[0]+120,roomCenter[1]}, vec2(40, 60), Textures::BODYWASH);
    Entity toilet = spawnEntity(vec2(roomCenter.x-390,roomCenter.y+165), vec2(275*0.785, 275), Textures::TOILET,-5);
    registry.toilets.emplace(toilet);

    
    printf("Bathroom roomMax: %f, %f\n", this->roomMax.x, this->roomMax.y);
    printf("Bathroom roomMin: %f, %f\n", this->roomMin.x, this->roomMin.y);

    bathroomGameSystem->getBathroom(this);
}


//TODO: after cleaning buddy for a while, clean tools will be used up
void BathRoom::cleanEvent() {

}


void BathRoom::clearPoopEntity(){


}

void BathRoom::triggerBathroomGame(Entity entity) {
    std::cout << "Bathroom game start!\n";
    registry.remove_all_components_of(entity);
    poopExists = false;
    worldSystem->changeRoom(-2, -1);
    bathroomGameSystem->beginGame();
}



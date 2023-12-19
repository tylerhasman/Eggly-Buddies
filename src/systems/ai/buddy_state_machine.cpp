//
// Created by Tyler on 2023-11-19.
//

#include "buddy_state_machine.h"
#include "../gameplay/gameplay_systems.h"
#include "systems/rooms/bath_room.h"

BuddyIdleState::BuddyIdleState(const Entity &buddy) : BuddyState(buddy)  {

}

int BuddyIdleState::step(float elapsed_ms) {

    idleDuration -= elapsed_ms / 1000.0f;

    if(idleDuration < 0.0f){

        if(buddyStatusSystem->isDead()){
            return BuddyState::States::Dead;
        }

        if(buddyStatusSystem->isReadyToEvolve()){
            return BuddyState::States::FollowPlayer;
        }

        if(buddyStatusSystem->satiation > 75.0f){
            return BuddyState::States::Poo;
        }

        if(buddyStatusSystem->growthStage == BuddyGrowthStage::Baby){
            float r = randomFloat();

            if(r < 0.9f){
                return BuddyState::States::FollowPlayer;
            }

            return BuddyState::States::Idle;
        }

        float r = randomFloat();
        if(r < 0.3f){
            return BuddyState::States::WanderRandom;
        }else if(r < 0.5f){
            return BuddyState::States::FollowPlayer;
        }else if(r < 0.8f) {
            return States::FindRandomPickup;
        }
        return States::Idle;
    }

    jumpTime -= elapsed_ms / 1000.0f;

    Motion& buddyMotion = getBuddyMotion();

    if(isBuddyOnGround()){
        if(jumpTime < 0.0f){
            jumpTime = randomFloat() * 2.0f + 1.5f;
            buddyMotion.velocity.y = -randomFloat() * 500.0f - 500.0f;
        }
    }

    return -1;
}

void BuddyIdleState::onStateEntered() {
    //idleDuration = randomFloat() * 10.0f + 5.0f;
    idleDuration = randomFloat() * 2.0f + 2.0f;
    jumpTime = randomFloat() + 1.0f;

    if(randomFloat() < 0.5f){
        direction = BuddyDirection::Left;
    }else{
        direction = BuddyDirection::Right;
    }

}

BuddyWanderRandomState::BuddyWanderRandomState(const Entity &buddy) : BuddyState(buddy) {
    selectedPosition = vec2(0.0);
}

int BuddyWanderRandomState::step(float elapsed_ms) {

    Motion& buddyMotion = getBuddyMotion();

    if(buddyMotion.position.x < selectedPosition.x){
        buddyMotion.position.x += 250.0f * elapsed_ms / 1000.0f;
    }else{
        buddyMotion.position.x -= 250.0f * elapsed_ms / 1000.0f;
    }

    if(glm::abs(buddyMotion.position.x - selectedPosition.x) < 100.0f){
        return States::Idle;
    }

    jumpTime -= elapsed_ms / 1000.0f;

    if(isBuddyOnGround()){
        if(jumpTime < 0.0f){
            jumpTime = randomFloat() * 5.0f;
            buddyMotion.velocity.y = -randomFloat() * 500.0f - 500.0f;
        }
    }
    return -1;
}

void BuddyWanderRandomState::onStateEntered() {
    selectedPosition = worldSystem->getRandomBuddyPosition();
    jumpTime = randomFloat() * 2.5f + 2.0f;
}

BuddyDirection BuddyWanderRandomState::getBuddyDirection() const {

    Motion& buddyMotion = getBuddyMotion();

    if(buddyMotion.position.x < selectedPosition.x) {
        return BuddyDirection::Right;
    }

    return BuddyDirection::Left;
}

BuddyNoAi::BuddyNoAi(const Entity &buddy) : BuddyState(buddy) {
 
}

BuddyFollowPlayer::BuddyFollowPlayer(const Entity &buddy) : BuddyState(buddy) {

}

int BuddyFollowPlayer::step(float elapsed_ms) {
    Motion& buddyMotion = getBuddyMotion();
    Motion& cameraPosition = registry.motions.get(worldSystem->getCamera());

    vec2 playerPosition = cameraPosition.position + vec2(game_width / 2.0f, 0.0f);

    if(buddyMotion.position.x < playerPosition.x){
        buddyMotion.position.x += 250.0f * elapsed_ms / 1000.0f;
    }else{
        buddyMotion.position.x -= 250.0f * elapsed_ms / 1000.0f;
    }

    if(glm::abs(buddyMotion.position.x - playerPosition.x) < 100.0f){
        bool isMinigameActive = WorldSystem::isMiniGameActive();

        if(buddyStatusSystem->isReadyToEvolve() && !isMinigameActive){
            return States::Evolve;
        }

        return States::Idle;
    }

    return -1;
}

void BuddyFollowPlayer::onStateEntered() {

}

BuddyPoo::BuddyPoo(const Entity& buddy) : BuddyState(buddy) {
    bathroomPosition = std::make_pair(vec2(0, 0), vec2(0, 0));
    bathroomDest = vec2(0, 0);
}

int BuddyPoo::step(float elapsed_ms) {
    Motion& buddyMotion = getBuddyMotion();

    Room* room = worldSystem->getRoomOfType<BathRoom>();
    BathRoom* bathroom = dynamic_cast<BathRoom*>(room);
    if (buddyMotion.position.x < bathroomDest.x) {
        buddyMotion.position.x += 250.0f * elapsed_ms / 1000.0f;
    }
    else {
        buddyMotion.position.x -= 250.0f * elapsed_ms / 1000.0f;
    }
    if (glm::abs(buddyMotion.position.x - bathroomDest.x) < 100.0f) {
       
        if (bathroom) {
            //minus the roomMin just to cancel out what we done in the function
            Entity poop = bathroom->spawnItem(buddyMotion.position-bathroom->roomMin, vec2(40, 40), Textures::POOP);
            bathroom->setPoopEntity(poop);
            registry.poops.emplace(poop);
            buddyStatusSystem->satiation -= 25.0f;
        } 
        return BuddyState::States::Idle;
    }
    return -1;
}

void BuddyPoo::onStateEntered() {
    bathroomPosition = worldSystem->getRoomPositionByType<BathRoom>();
    vec2 roomMin = bathroomPosition.first;
    vec2 roomMax = bathroomPosition.second;
    float middleX = (roomMax.x + roomMin.x) / 2.0f;
    bathroomDest = vec2(middleX, 0);
}


BuddyDirection BuddyFollowPlayer::getBuddyDirection() const {
    Motion& buddyMotion = getBuddyMotion();
    Motion& cameraPosition = registry.motions.get(worldSystem->getCamera());

    vec2 playerPosition = cameraPosition.position + vec2(game_width / 2.0f, 0.0f);

    return buddyMotion.position.x < playerPosition.x ? BuddyDirection::Right : BuddyDirection::Left;
}

int BuddyEvolveState::step(float elapsed_ms) {

    if(WorldSystem::isMiniGameActive()){
        std::cout << "Cancelling evolve because player went into a minigame!" << std::endl;
        return States::Idle;
    }

    if(jumpsLeft > 0){
        Motion& buddyMotion = getBuddyMotion();

        if(isBuddyOnGround()){
            buddyMotion.velocity.y = -randomFloat() * 500.0f - 500.0f;
            jumpsLeft--;
        }
    }else{
        buddyStatusSystem->evolveBuddy();
        return States::Idle;
    }

    return -1;
}

void BuddyEvolveState::onStateEntered() {
    jumpsLeft = 3;
}

BuddyDirection BuddyEvolveState::getBuddyDirection() const {
    return jumpsLeft % 2 == 0 ? BuddyDirection::Left : BuddyDirection::Right;
}

BuddyActionType BuddyEvolveState::getBuddyAction() const {
    return BuddyActionType::Idle;
}

int BuddyDeathState::step(float elapsed_ms) {
    return -1;
}

void BuddyDeathState::onStateEntered() {
    Text& text = registry.texts.get(buddy);

    text.offset = vec2(-100, -100);
    text.text = "Your buddy has died!\nClick on it to start again";
    text.color = vec3(0.85f);
    text.scale = 0.5f;
    text.shadow = true;

    Clickable& clickable = registry.clickables.get(buddy);

    clickable.highlightOnHover = true;
    clickable.onClick = [](Entity ent){
        buddyStatusSystem->satiation = 100.0f;
        buddyStatusSystem->rest = 100.0f;
        buddyStatusSystem->hygiene = 100.0f;
        buddyStatusSystem->entertainment = 100.0f;
        buddyStatusSystem->age = 0.0f;
        buddyStatusSystem->personality = Personality::Baby;
        buddyStatusSystem->growthStage = BuddyGrowthStage::Baby;
        SaveSystem::saveBuddyStatus();

        worldSystem->restart_game();
    };

}

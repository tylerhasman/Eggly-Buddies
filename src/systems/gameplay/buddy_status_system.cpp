#include "buddy_status_system.h"
#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include <iostream>
#include <sstream>

void BuddyStatusSystem::step(float elapsed_ms) {

    if(!WorldSystem::isMiniGameActive()){
        if(!worldSystem->isIntroCutScene){
            updateEgglyStatus(elapsed_ms);
        }
    }

	updateBar(elapsed_ms);
}

void BuddyStatusSystem::updateBar(float elapsed_ms) {
	for (auto& entity : registry.statusBars.entities) {
		if (registry.hygiene.has(entity)) {
			//update hygiene bar
			RenderRequest& barRequest = registry.renderRequests.get(entity);
			barRequest.iUniforms["u_EnableBar"] = 1;
			barRequest.fUniforms["u_barFloat"] = hygiene;
			barRequest.iUniforms["u_barIndicator"] = 1;
		} else if (registry.rest.has(entity)) {
			//update rest bar
			RenderRequest& barRequest = registry.renderRequests.get(entity);
			barRequest.iUniforms["u_EnableBar"] = 1;
			barRequest.fUniforms["u_barFloat"] = rest;
			barRequest.iUniforms["u_barIndicator"] = 2;
		} else if (registry.satiation.has(entity)) {
			//update satiation bar
			RenderRequest& barRequest = registry.renderRequests.get(entity);
			barRequest.iUniforms["u_EnableBar"] = 1;
			barRequest.fUniforms["u_barFloat"] = satiation;
			barRequest.iUniforms["u_barIndicator"] = 3;
		} else if (registry.entertainment.has(entity)) {
			//update satiation bar
			RenderRequest& barRequest = registry.renderRequests.get(entity);
			barRequest.iUniforms["u_EnableBar"] = 1;
			barRequest.fUniforms["u_barFloat"] = entertainment;  //entertainment
			barRequest.iUniforms["u_barIndicator"] = 4;
		}
	}



}


void BuddyStatusSystem::updateEgglyStatus(float elapsed_ms) {
	if (hygiene >= MIN_STAT) {
		hygiene -= 0.5f * elapsed_ms / 1000;
		// event for hygiene change	
			// event 1: backpard playing -> increases
			// event 2: cooking -> increases 
			// event 3: washing -> decreases
	}

	if (rest >= MIN_STAT) {
		rest -= 0.5f * elapsed_ms / 1000;
		// event for rest change
			// event 1: backyard playing -> increases
			// event 2: cooking -> increases
			// event 3: sleeping -> decreases

	}

	if (satiation >= MIN_STAT) {
		satiation -= 0.5f * elapsed_ms / 1000;
		// event for satiation change
			// event 1: backpard playing -> increases 
			// event 2: cooking -> decreases
			// event 3: sleeping -> increases
	}


	if (entertainment >= MIN_STAT) {
		entertainment -= 0.5f * elapsed_ms / 1000;
		// event for entertainment change
			// event 1: backpard playing -> decreases
	}

    age += elapsed_ms / 1000.0f;

    hygiene = glm::clamp(hygiene, MIN_STAT, MAX_STAT);
    rest = glm::clamp(rest, MIN_STAT, MAX_STAT);
    satiation = glm::clamp(satiation, MIN_STAT, MAX_STAT);
    entertainment = glm::clamp(entertainment, MIN_STAT, MAX_STAT);
}

void BuddyStatusSystem::getBuddyLook(BuddyActionType actionType, Textures &texture, SpriteSheet &spriteSheet) {

    if(buddyTextures.find(personality) == buddyTextures.end()){
        texture = Textures::TEST_GUY;
        spriteSheet.dimensions = vec2(1, 1);
        return;
    }

    auto& map = buddyTextures[personality];

    if(map.find(growthStage) == map.end()){
        texture = Textures::TEST_GUY;
        spriteSheet.dimensions = vec2(1, 1);
        return;
    }

    auto& growthStageMap = map[growthStage];

    if(growthStageMap.find(actionType) == growthStageMap.end()){
        texture = Textures::TEST_GUY;
        spriteSheet.dimensions = vec2(1, 1);
        return;
    }

    texture = buddyTextures[personality][growthStage][actionType];

    switch(actionType){
        case BuddyActionType::Idle:
        case BuddyActionType::Run:
        case BuddyActionType::Sleep:
            spriteSheet.dimensions = vec2(2, 1);
            break;
        case BuddyActionType::Dead:
        case BuddyActionType::Jump:
            spriteSheet.dimensions = vec2(1, 1);
            break;
    }

}

void BuddyStatusSystem::initBuddyTextures() {
    buddyTextures[Personality::Baby][BuddyGrowthStage::Baby][BuddyActionType::Dead] = Textures::BUDDY_BABY_DEATH;
    buddyTextures[Personality::Baby][BuddyGrowthStage::Baby][BuddyActionType::Idle] = Textures::BUDDY_BABY_IDLE;
    buddyTextures[Personality::Baby][BuddyGrowthStage::Baby][BuddyActionType::Jump] = Textures::BUDDY_BABY_JUMP;
    buddyTextures[Personality::Baby][BuddyGrowthStage::Baby][BuddyActionType::Run] = Textures::BUDDY_BABY_RUN;
    buddyTextures[Personality::Baby][BuddyGrowthStage::Baby][BuddyActionType::Sleep] = Textures::BUDDY_BABY_SLEEP;

    buddyTextures[Personality::Smart][BuddyGrowthStage::Teen][BuddyActionType::Dead] = Textures::BUDDY_SMART_DEATH;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Teen][BuddyActionType::Idle] = Textures::BUDDY_SMART_IDLE;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Teen][BuddyActionType::Jump] = Textures::BUDDY_SMART_JUMP;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Teen][BuddyActionType::Run] = Textures::BUDDY_SMART_RUN;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Teen][BuddyActionType::Sleep] = Textures::BUDDY_SMART_SLEEP;

    buddyTextures[Personality::Cute][BuddyGrowthStage::Teen][BuddyActionType::Dead] = Textures::BUDDY_CUTE_DEATH;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Teen][BuddyActionType::Idle] = Textures::BUDDY_CUTE_IDLE;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Teen][BuddyActionType::Jump] = Textures::BUDDY_CUTE_JUMP;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Teen][BuddyActionType::Run] = Textures::BUDDY_CUTE_RUN;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Teen][BuddyActionType::Sleep] = Textures::BUDDY_CUTE_SLEEP;

    buddyTextures[Personality::Jock][BuddyGrowthStage::Teen][BuddyActionType::Dead] = Textures::BUDDY_JOCK_DEATH;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Teen][BuddyActionType::Idle] = Textures::BUDDY_JOCK_IDLE;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Teen][BuddyActionType::Jump] = Textures::BUDDY_JOCK_JUMP;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Teen][BuddyActionType::Run] = Textures::BUDDY_JOCK_RUN;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Teen][BuddyActionType::Sleep] = Textures::BUDDY_JOCK_SLEEP;

    buddyTextures[Personality::Cool][BuddyGrowthStage::Teen][BuddyActionType::Dead] = Textures::BUDDY_COOL_DEATH;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Teen][BuddyActionType::Idle] = Textures::BUDDY_COOL_IDLE;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Teen][BuddyActionType::Jump] = Textures::BUDDY_COOL_JUMP;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Teen][BuddyActionType::Run] = Textures::BUDDY_COOL_RUN;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Teen][BuddyActionType::Sleep] = Textures::BUDDY_COOL_SLEEP;

    buddyTextures[Personality::Golden][BuddyGrowthStage::Teen][BuddyActionType::Dead] = Textures::BUDDY_GOLD_DEATH;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Teen][BuddyActionType::Idle] = Textures::BUDDY_GOLD_IDLE;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Teen][BuddyActionType::Jump] = Textures::BUDDY_GOLD_JUMP;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Teen][BuddyActionType::Run] = Textures::BUDDY_GOLD_RUN;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Teen][BuddyActionType::Sleep] = Textures::BUDDY_GOLD_SLEEP;

    buddyTextures[Personality::Hobo][BuddyGrowthStage::Teen][BuddyActionType::Dead] = Textures::BUDDY_HOBO_DEATH;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Teen][BuddyActionType::Idle] = Textures::BUDDY_HOBO_IDLE;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Teen][BuddyActionType::Jump] = Textures::BUDDY_HOBO_JUMP;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Teen][BuddyActionType::Run] = Textures::BUDDY_HOBO_RUN;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Teen][BuddyActionType::Sleep] = Textures::BUDDY_HOBO_SLEEP;

    buddyTextures[Personality::Smart][BuddyGrowthStage::Adult][BuddyActionType::Dead] = Textures::BUDDY_ADULT_SMART_DEATH;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Adult][BuddyActionType::Idle] = Textures::BUDDY_ADULT_SMART_IDLE;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Adult][BuddyActionType::Jump] = Textures::BUDDY_ADULT_SMART_JUMP;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Adult][BuddyActionType::Run] = Textures::BUDDY_ADULT_SMART_RUN;
    buddyTextures[Personality::Smart][BuddyGrowthStage::Adult][BuddyActionType::Sleep] = Textures::BUDDY_ADULT_SMART_SLEEP;

    buddyTextures[Personality::Cute][BuddyGrowthStage::Adult][BuddyActionType::Dead] = Textures::BUDDY_ADULT_CUTE_DEATH;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Adult][BuddyActionType::Idle] = Textures::BUDDY_ADULT_CUTE_IDLE;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Adult][BuddyActionType::Jump] = Textures::BUDDY_ADULT_CUTE_JUMP;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Adult][BuddyActionType::Run] = Textures::BUDDY_ADULT_CUTE_RUN;
    buddyTextures[Personality::Cute][BuddyGrowthStage::Adult][BuddyActionType::Sleep] = Textures::BUDDY_ADULT_CUTE_SLEEP;

    buddyTextures[Personality::Jock][BuddyGrowthStage::Adult][BuddyActionType::Dead] = Textures::BUDDY_ADULT_JOCK_DEATH;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Adult][BuddyActionType::Idle] = Textures::BUDDY_ADULT_JOCK_IDLE;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Adult][BuddyActionType::Jump] = Textures::BUDDY_ADULT_JOCK_JUMP;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Adult][BuddyActionType::Run] = Textures::BUDDY_ADULT_JOCK_RUN;
    buddyTextures[Personality::Jock][BuddyGrowthStage::Adult][BuddyActionType::Sleep] = Textures::BUDDY_ADULT_JOCK_SLEEP;

    buddyTextures[Personality::Cool][BuddyGrowthStage::Adult][BuddyActionType::Dead] = Textures::BUDDY_ADULT_COOL_DEATH;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Adult][BuddyActionType::Idle] = Textures::BUDDY_ADULT_COOL_IDLE;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Adult][BuddyActionType::Jump] = Textures::BUDDY_ADULT_COOL_JUMP;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Adult][BuddyActionType::Run] = Textures::BUDDY_ADULT_COOL_RUN;
    buddyTextures[Personality::Cool][BuddyGrowthStage::Adult][BuddyActionType::Sleep] = Textures::BUDDY_ADULT_COOL_SLEEP;

    buddyTextures[Personality::Golden][BuddyGrowthStage::Adult][BuddyActionType::Dead] = Textures::BUDDY_ADULT_GOLD_DEATH;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Adult][BuddyActionType::Idle] = Textures::BUDDY_ADULT_GOLD_IDLE;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Adult][BuddyActionType::Jump] = Textures::BUDDY_ADULT_GOLD_JUMP;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Adult][BuddyActionType::Run] = Textures::BUDDY_ADULT_GOLD_RUN;
    buddyTextures[Personality::Golden][BuddyGrowthStage::Adult][BuddyActionType::Sleep] = Textures::BUDDY_ADULT_GOLD_SLEEP;

    buddyTextures[Personality::Hobo][BuddyGrowthStage::Adult][BuddyActionType::Dead] = Textures::BUDDY_ADULT_HOBO_DEATH;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Adult][BuddyActionType::Idle] = Textures::BUDDY_ADULT_HOBO_IDLE;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Adult][BuddyActionType::Jump] = Textures::BUDDY_ADULT_HOBO_JUMP;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Adult][BuddyActionType::Run] = Textures::BUDDY_ADULT_HOBO_RUN;
    buddyTextures[Personality::Hobo][BuddyGrowthStage::Adult][BuddyActionType::Sleep] = Textures::BUDDY_ADULT_HOBO_SLEEP;
}

vec2 BuddyStatusSystem::getBuddySize() const {
    switch(growthStage){
        case BuddyGrowthStage::Baby:
            return vec2(128, 100) * 0.8f;
        case BuddyGrowthStage::Teen:
            return vec2(130, 120);
        case BuddyGrowthStage::Adult:
            return vec2(140.7f, 120) * 1.2f;
    }

    return vec2(128, 100);
}

bool BuddyStatusSystem::isReadyToEvolve() {
    if(growthStage == BuddyGrowthStage::Baby){
        if(age > teenGrowthAge){
            return true;
        }
    }else if(growthStage == BuddyGrowthStage::Teen){
        if(age > adultGrowthAge){
            return true;
        }
    }
    return false;
}

void BuddyStatusSystem::evolveBuddy() {
    if(growthStage == BuddyGrowthStage::Baby) {
        growthStage = BuddyGrowthStage::Teen;

        std::vector<Personality> possiblePersonalities;
        possiblePersonalities.push_back(Personality::Smart);
        possiblePersonalities.push_back(Personality::Cute);
        possiblePersonalities.push_back(Personality::Cool);
        possiblePersonalities.push_back(Personality::Jock);

        if (rest < 50) {
            possiblePersonalities.erase(
                    std::remove(possiblePersonalities.begin(), possiblePersonalities.end(), Personality::Smart),
                    possiblePersonalities.end());
        }
        if (hygiene < 50) {
            possiblePersonalities.erase(
                    std::remove(possiblePersonalities.begin(), possiblePersonalities.end(), Personality::Cute),
                    possiblePersonalities.end());
        }
        if (satiation < 50) {
            possiblePersonalities.erase(
                    std::remove(possiblePersonalities.begin(), possiblePersonalities.end(), Personality::Jock),
                    possiblePersonalities.end());
        }
        if (entertainment < 50) {
            possiblePersonalities.erase(
                    std::remove(possiblePersonalities.begin(), possiblePersonalities.end(), Personality::Cool),
                    possiblePersonalities.end());
        }

        if (possiblePersonalities.size() >= 4) {
            personality = Personality::Golden;
        } else if (possiblePersonalities.empty()) {
            personality = Personality::Hobo;
        } else {
            personality = *select_randomly(possiblePersonalities.begin(), possiblePersonalities.end());
        }
    }else if(growthStage == BuddyGrowthStage::Teen){
        growthStage = BuddyGrowthStage::Adult;
    }else{
        std::cerr << "Can't evolve buddy, it's already at max!" << std::endl;
    }
}

bool BuddyStatusSystem::isDead() const {
    return satiation <= 0.0 && rest <= 0.0 && entertainment <= 0.0 && hygiene <= 0.0;
}

#include "kitchen_game_system.h"
#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <sstream>

Entity caughtScoreEntity;
Entity missedScoreEntity;
Entity instructionsEntity;

void KitchenGameSystem::step(float elapsed_ms) {

    //displayMessageTimer -= elapsed_ms / 1000.0f;

    /*if (displayMessageTimer < 0.f) {
        registry.remove_all_components_of(displayText);
    }*/

    if(isRunning){
        if (pancakesMissed >= 5) {
            displayEndMsg(false);

            endGame();
            return;
        }

        foodFallingTimer -= elapsed_ms / 1000.0f;
        displayInstructions(elapsed_ms);

        if(foodFallingTimer < 0.0f){
            foodFallingTimer += 5.0f * (uniform_dist(rng) + 0.1f);

            spawnFallingFood();
        }

        Motion& playerMotion = registry.motions.get(plate);

        if(isRightHeld ^ isLeftHeld){
            if(isRightHeld){
                playerMotion.velocity.x += playerMoveSpeed * elapsed_ms / 1000.0f;
            }

            if(isLeftHeld){
                playerMotion.velocity.x -= playerMoveSpeed * elapsed_ms / 1000.0f;
            }
        }

        if(playerMotion.position.x - playerMotion.scale.x / 2.0f < room->roomMin.x){
            playerMotion.position.x = room->roomMin.x + playerMotion.scale.x / 2.0f;
            playerMotion.velocity.x *= -1 * 0.8f;
        }

        if(playerMotion.position.x + playerMotion.scale.x / 2.0f > room->roomMax.x){
            playerMotion.position.x = room->roomMax.x - playerMotion.scale.x / 2.0f;
            playerMotion.velocity.x *= -1 * 0.8f;
        }

        for(const Entity& fallingFood : registry.fallingFoods.entities){
            FallingFood& fallingFoodComponent = registry.fallingFoods.get(fallingFood);
            Motion& fallingFoodMotion = registry.motions.get(fallingFood);

            if(fallingFoodComponent.isStackedOnSomething){

                Entity& stackedOn = fallingFoodComponent.stackedOn;

                Motion& catcherMotion = registry.motions.get(stackedOn);

                fallingFoodMotion.position = catcherMotion.position;
                fallingFoodMotion.position.x += fallingFoodComponent.offsetX;
                fallingFoodMotion.position.y -= pancake_size.y / 2.f;

                fallingFoodComponent.offsetX += fallingFoodComponent.velocityX * elapsed_ms / 1000.0f;

                float maxLean = glm::max(75.0f - (float) (fallingFoodComponent.numberInStack - 1) * 7.5f, 3.0f);

                fallingFoodComponent.offsetX = glm::clamp(fallingFoodComponent.offsetX, -maxLean, maxLean);

                const float drag = (float) fallingFoodComponent.numberInStack * -0.035f;

                if(isRightHeld ^ isLeftHeld){
                    if(isRightHeld){
                        fallingFoodComponent.velocityX += drag * playerMoveSpeed * elapsed_ms / 1000.0f;
                    }

                    if(isLeftHeld){
                        fallingFoodComponent.velocityX -= drag * playerMoveSpeed * elapsed_ms / 1000.0f;
                    }

                }

                fallingFoodComponent.velocityX = glm::clamp(fallingFoodComponent.velocityX, -10.0f, 10.0f);

            }

            if (fallingFoodMotion.position.y < room->roomMin.y) {
                registry.remove_all_components_of(fallingFood);
                pancakesMissed++;
                std::cout << "Missed Pancakes: " << pancakesMissed << std::endl;
	            
                std::stringstream ss;
                ss << "Missed Pancakes: " << pancakesMissed;
                registry.texts.get(missedScoreEntity).text = ss.str().c_str();

            } else if (fallingFoodMotion.position.y > room->roomMax.y) {
                registry.remove_all_components_of(fallingFood);
                pancakesMissed++;
                std::cout << "Missed Pancakes: " << pancakesMissed << std::endl;

                std::stringstream ss;
                ss << "Missed Pancakes: " << pancakesMissed;
                registry.texts.get(missedScoreEntity).text = ss.str().c_str();
            }
        }

    }
}

void KitchenGameSystem::displayInstructions(float elapsed_ms) {
	if (isShowingInstructions) {
		curr_instruction_timer -= elapsed_ms;
		if (curr_instruction_timer < 0) {
			stopInstructions();
		}
	}
}

void KitchenGameSystem::stopInstructions() {
	isShowingInstructions = false;
	curr_instruction_timer = INSTRUCTION_TIMER;
	registry.remove_all_components_of(instructionsEntity);
}

void KitchenGameSystem::beginGame() {
    if(room == nullptr){
        std::cerr << "Kitchen game room is null!" << std::endl;
        return;
    }
	displayUI(false);

    isRunning = true;
    isShowingInstructions = true;
    hasPlateBeenStackedOn = false;
    pancakesCaught = 0;
    pancakesMissed = 0;
    score = 0;

    std::cout << "satiation at beginning of game: " << buddyStatusSystem->satiation << std::endl;

    caughtScoreEntity = create_sprite_entity(renderer, room->roomCenter, vec2(0, 0), Textures::PANCAKE, false, -10);
	Text textComp = {
		"Score: 0",
		vec2(650.f, -1150.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f};
	registry.texts.emplace(caughtScoreEntity, textComp);

    missedScoreEntity = create_sprite_entity(renderer, room->roomCenter, vec2(0, 0), Textures::PANCAKE, false, -10);
	Text missedTextComp = {
		"Missed Pancakes: 0",
		vec2(650.f, -1100.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f};
	registry.texts.emplace(missedScoreEntity, missedTextComp);

	instructionsEntity = create_sprite_entity(renderer, room->roomCenter, vec2(0, 0), Textures::PANCAKE, false, -10);
	Text instrText = {
		"PRESS LEFT & RIGHT ARROW KEYS TO CATCH PANCAKES",
		vec2(800.f, -850.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f };
	registry.texts.emplace(instructionsEntity, instrText);

}

void KitchenGameSystem::increaseSatiaton() {
    buddyStatusSystem->satiation += score;
}

void KitchenGameSystem::displayEndMsg(bool isWin) {

    //displayMessageTimer = 3.0f;

    if(isWin){
        //displayText = room->spawnEntity(room->roomCenter + vec2(0.0, room->roomSize.y), vec2(180, 110) * 5.0f, Textures::WIN_MSG, -5);
        chatboxSystem->createBox("Congrats you win!");
        std::cout << "You win!" << std::endl;
    } else {
        //displayText = room->spawnEntity(room->roomCenter + vec2(0.0, room->roomSize.y), vec2(180, 110) * 5.0f, Textures::LOSE_MSG, -5);
        chatboxSystem->createBox("You suck :( Try again!");
        std::cout << "You lose!" << std::endl;
    }

}


void KitchenGameSystem::endGame() {

    for(const Entity& food : registry.fallingFoods.entities){
        registry.remove_all_components_of(food);
    }
    fallingFoods.clear();

    registry.remove_all_components_of(caughtScoreEntity);
    registry.remove_all_components_of(missedScoreEntity);
    registry.remove_all_components_of(instructionsEntity);

    registry.remove_all_components_of(plate);

    increaseSatiaton();
    std::cout << "satiation at end of game: " << buddyStatusSystem->satiation << std::endl;

    isRunning = false;
    stopInstructions();
    displayUI(true);
	worldSystem->changeRoom(1, 0);
}

void KitchenGameSystem::setupKitchenGame(Room *gameRoom, Entity playerEntity) {
    room = gameRoom;
    plate = playerEntity;
}

void KitchenGameSystem::spawnFallingFood() {
    
    vec2 pancakeCoords = vec2(uniform_dist(rng) * (room->roomSize.x - 200.0f) + 100.0f, room->roomSize.y - 750);
    Entity pancake = room->spawnEntity(pancakeCoords, pancake_size, Textures::PANCAKE, -1);
    
    CollisionHandler& pancakeCollision = registry.collisionHandlers.emplace(pancake);
    pancakeCollision.onCollision = [this, pancake](Entity, Collision collision) {

        bool canCatch = false;

        FallingFood& thisFood = registry.fallingFoods.get(pancake);

        if(!thisFood.isStackedOnSomething) {
            if(collision.other_entity == plate){
                if(!hasPlateBeenStackedOn){
                    canCatch = true;
                    hasPlateBeenStackedOn = true;
                } 
            }

            if(registry.fallingFoods.has(collision.other_entity)){
                FallingFood& otherFood = registry.fallingFoods.get(collision.other_entity);
                if(otherFood.isStackableOn){
                    canCatch = true;
                }
            }

            if(canCatch) {
                handleCatchFood(pancake, collision.other_entity);
            }
        }
    };

    FallingFood fallingFood = {};
    registry.fallingFoods.insert(pancake, fallingFood);
    Motion& motion = registry.motions.get(pancake); //initial position, constant velocity
    motion.velocity = pancake_speed;
}

void KitchenGameSystem::playerInput(bool moveLeft, bool moveRight) {
    isLeftHeld = moveLeft;
    isRightHeld = moveRight;
}

void KitchenGameSystem::handleCatchFood(Entity food, Entity catcher) {

	incrementScore();

    pancakesCaught++;

    if(pancakesCaught >= 10){
        displayEndMsg(true);

        // buddyStatusSystem->hunger = 0.0f;
        buddyStatusSystem->satiation = 100.0f;

        endGame();
        return;
    }
	
	// "attach" food to plate
	registry.renderRequests.get(food).z_index = 1;

	Motion& pancakeMotion = registry.motions.get(food);

    pancakeMotion.velocity = vec2(0.f, 0.f);

    FallingFood& fallingFood = registry.fallingFoods.get(food);

    fallingFood.stackedOn = catcher;
    fallingFood.isStackableOn = true;
    fallingFood.isStackedOnSomething = true;
    fallingFood.numberInStack = 1;

    if(registry.fallingFoods.has(catcher)){
        FallingFood& catcherFood = registry.fallingFoods.get(catcher);
        catcherFood.isStackableOn = false;

        fallingFood.numberInStack = catcherFood.numberInStack + 1;

    }

}

void KitchenGameSystem::incrementScore() {
	score += 10;
	std::stringstream ss;
	ss << "Score: " << score;
	std::cout << ss.str().c_str() << std::endl;
    registry.texts.get(caughtScoreEntity).text = ss.str().c_str();
}

void KitchenGameSystem::displayUI(bool isDisplay) {
	if (!isDisplay) {
		for (Entity entity : registry.userInterfaces.entities) {
			RenderRequest& renderReq = registry.renderRequests.get(entity);
			renderReq.z_index += tempUIZIncrement;
		}
	}
	else {
		for (Entity entity : registry.userInterfaces.entities) {
			RenderRequest& renderReq = registry.renderRequests.get(entity);
			renderReq.z_index -= tempUIZIncrement;
		}
	}
}

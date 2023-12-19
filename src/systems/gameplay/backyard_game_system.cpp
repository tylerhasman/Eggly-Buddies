#include "backyard_game_system.h"
#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/ai/enemy_state_machine.h"



#include <iostream>
#include <sstream>
#include <cmath>

Entity station;

void BackyardGameSystem::step(float elapsed_ms) {

    if (isRunning) {
		Motion& stationMotion = registry.motions.get(station);
		Motion& gunMotion = registry.motions.get(gun);
    	vec2 buddySize = buddyStatusSystem->getBuddySize() * 0.5f;
		if (mousePosition.x - stationMotion.position.x > 0) {
			stationMotion.scale.x = glm::abs(stationMotion.scale.x);
			gunMotion.scale.x = glm::abs(gunMotion.scale.x);
			gunMotion.angle = atan2(mousePosition.y - stationMotion.position.y, mousePosition.x - stationMotion.position.x);
		} else {
			stationMotion.scale.x = -glm::abs(stationMotion.scale.x);
			gunMotion.scale.x = -glm::abs(gunMotion.scale.x);
			gunMotion.angle = atan2(-(mousePosition.y - stationMotion.position.y), -(mousePosition.x - stationMotion.position.x));
		}
        float gun_angle = atan2(mousePosition.y - stationMotion.position.y, mousePosition.x - stationMotion.position.x);
    	gunMotion.position.x = stationMotion.position.x + 25 * cos(gun_angle);
    	gunMotion.position.y = stationMotion.position.y + 25 * sin(gun_angle);

		for(uint i = 0; i < registry.hitTimers.size(); i++) {
			HitTimer& hitTimer = registry.hitTimers.components[i];
			if (hitTimer.timer < 0) {
				Entity& enemyEntity = registry.hitTimers.entities[i];
				BackyardEnemy& backyardEnemy = registry.backyardEnemies.get(enemyEntity); 
				Entity& imageOverlay = backyardEnemy.imageOverlay;
				SpriteSheet& spriteSheet = registry.spriteSheets.get(imageOverlay);
				spriteSheet.dimensions = vec2(2,1);
				RenderRequest& rRequest = registry.renderRequests.get(imageOverlay);
				rRequest.used_texture = Textures::SQUIRREL_SPRITESHEET;
				registry.hitTimers.remove(registry.hitTimers.entities[i]);
			} else {
				hitTimer.timer -= elapsed_ms / 1000;
			}
		}

		timer -=  (elapsed_ms / 1000);
		std::stringstream ss;
		ss << "Timer: " << round(timer);
		registry.texts.get(timerEntity).text = ss.str().c_str();
		// registry.texts.get(timerEntity).offset = vec2(50.f, 50.f);
		// registry.texts.get(timerEntity).text = "Timer: " + std::to_string(round(timer));
		if (registry.backyardEnemies.size() <= (size_t) 0) {
			if (backyardLevel <= 5) {
				backyardLevel += 1;
			}
			 
        	chatboxSystem->createBox("Congrats you win!");
        	std::cout << "You win!" << std::endl;
        	endGame();
			return;
		}

		if (isShowingInstructions) {
			if (timer <= INSTRUCTION_CUTOFF) {
				stopInstructions();
			}
		}

		if (timer <= 0) {
			chatboxSystem->createBox("You ran out of time :( Try again!");
        	std::cout << "You lose!" << std::endl;
			endGame();
			return;
		}
	}

	

}

void BackyardGameSystem::beginGame() {
	std::cout << "BACKYARD: Begin" << std::endl;
	
	roomC = vec2(roomMin.x + roomS.x / 2, roomMin.y + roomS.y / 2);

    vec2 buddySize = buddyStatusSystem->getBuddySize() * 0.5f;

	station = create_sprite_entity(renderer, roomC, buddySize, Textures::TEST_GUY, false, -1);
	// registry.players.emplace(station);
	ammo1 = create_sprite_entity(renderer, roomC + vec2(-10, - buddySize.y), vec2(16,16), Textures::BULLET, false, 0);
	ammo2 = create_sprite_entity(renderer, roomC + vec2(10, - buddySize.y), vec2(16,16), Textures::BULLET, false, 0);


	RenderRequest& playerRenderRequest = registry.renderRequests.get(station);
	
	SpriteSheet& spriteSheet = registry.spriteSheets.emplace(station);
	spriteSheet.dimensions = vec2(2, 1);
	spriteSheet.current = vec2(0, 0);
	spriteSheet.frameSpeed = 0.5f;

    buddyStatusSystem->getBuddyLook(BuddyActionType::Idle, playerRenderRequest.used_texture, spriteSheet);

	gun = create_sprite_entity(renderer, roomC, vec2(42, 27), Textures::GUN, false, -2);

	handler.onCollision = [&](Entity e1, Collision collision) {
        if(registry.projectiles.has(collision.other_entity)){
			AiController& controller = registry.aiControllers.get(e1);
			controller.stateMachine.enterState(EnemyStates::Return);

            // pickupMotion.position = pickupMotion.lastPhysicsPosition;

            // vec2 reflectedVelocity = pickupMotion.velocity - 2.0f * glm::dot(pickupMotion.velocity, collision.normal) * collision.normal;

            // // Apply the reflected velocity
            // pickupMotion.velocity = glm::normalize(reflectedVelocity);
            // Motion& projectileMotion = registry.motions.get(collision.other_entity);
			// registry.projectiles.remove(collision.other_entity);
			// registry.renderRequests.remove(collision.other_entity);
			registry.remove_all_components_of(collision.other_entity);
			if (registry.motions.has(ammo1) && registry.motions.has(ammo2)) {
				if (noBullet) {
					Motion& ammo1Motion = registry.motions.get(ammo1);
					ammo1Motion.scale = vec2(16,16);
					noBullet = false;
					backyardGameSystem->lastBullet = true;
				} else if (lastBullet) {
					Motion& ammo2Motion = registry.motions.get(ammo2);
					ammo2Motion.scale = vec2(16,16);
					lastBullet = false;
				}
			}
			takeDamage(e1);
        }
    };

	for (int i = 0; i < (backyardLevel % 2) + 2; i++) {
		
	}

	int spawnTimes;

	if (backyardLevel <= 3) {
    	spawnTimes = 3 + (backyardLevel - 1);
	} else {
    	// level 4 = 3 enemies, level 5 = 4 enemies, level 6 = 5 enemies
    	spawnTimes = (backyardLevel % 3 == 1) ? 3 : ((backyardLevel % 3 == 2) ? 4 : 5);
	}	

	float angle_section = 2 * M_PI / spawnTimes;

	for (int i = 0; i < spawnTimes; ++i) {
    	createEnemyAI(i * angle_section);
	}

	// createEnemyAI(0);
	// createEnemyAI(M_PI/2);
	// createEnemyAI(M_PI);
	// createEnemyAI(3*M_PI/2);

	displayUI(false);

	isRunning = true;

	scoreEntity = create_sprite_entity(renderer, roomMin, vec2(0, 0), Textures::SQUIRREL_1, false, -10);
	Text scoreTextComp = {
		"Score: 0",
		vec2(50.f, 50.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f};
	registry.texts.emplace(scoreEntity, scoreTextComp);

	timerEntity = create_sprite_entity(renderer, vec2(roomMin.x + roomS.x, roomMin.y), vec2(0, 0), Textures::SQUIRREL_1, false, -10);
	Text timerTextComp = {
		"Timer: 60",
		vec2(-200.f, 50.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f};
	registry.texts.emplace(timerEntity, timerTextComp);

	instructionEntity = create_sprite_entity(renderer, vec2(roomC.x, roomMin.y), vec2(0, 0), Textures::SQUIRREL_1, false, -10);
	if (backyardLevel <= 3) {
		Text instrText = {
		"CLICK TO SHOOT SQUIRRELS BEFORE TIME RUNS OUT\nONLY 2 PROJECTILES ON THE SCREEN AT A TIME",
		vec2(-225, 15),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.3f };
		registry.texts.emplace(instructionEntity, instrText);

	} else {
		Text instrText = {
		"SQUIRRELS CAN NOW DODGE PROJECTILES",
		vec2(-200, 25),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.35f };
		registry.texts.emplace(instructionEntity, instrText);
	}

	isShowingInstructions = true;
	timer = 60.f;
}

void BackyardGameSystem::createEnemyAI(float angle) {
	Entity meshBoi = createSquirrel(renderer, vec2(roomC.x + 300 * cos(angle), roomC.y + 300 * sin(angle)));
			
	BackyardEnemy& backyardEnemy = registry.backyardEnemies.emplace(meshBoi);
	registry.collisionHandlers.emplace(meshBoi, handler);
    registry.motions.get(meshBoi).enableMeshCollision = true;

	AiController& controller = registry.aiControllers.emplace(meshBoi);

	controller.stateMachine.addState(EnemyStates::Idle, std::make_shared<EnemyIdleState>(meshBoi));
	controller.stateMachine.addState(EnemyStates::Moving, std::make_shared<EnemyMovingState>(meshBoi, roomC, roomS, angle));
	controller.stateMachine.addState(EnemyStates::Dodge, std::make_shared<EnemyDodgeState>(meshBoi));
	controller.stateMachine.addState(EnemyStates::Block, std::make_shared<EnemyBlockState>(meshBoi));
	controller.stateMachine.addState(EnemyStates::Return, std::make_shared<EnemyReturnState>(meshBoi));
	controller.stateMachine.addState(EnemyStates::Backoff, std::make_shared<EnemyBackoffState>(meshBoi, roomC));


    controller.stateMachine.enterState(EnemyStates::Moving);

	Entity squirrelImage = createSquirrelImage(renderer, vec2(roomC.x + 300 * cos(angle), roomC.y + 300 * sin(angle)));

	backyardEnemy.imageOverlay = squirrelImage;

    // Motion& imageMotion = registry.motions.get(squirrelImage);
}

void BackyardGameSystem::takeDamage(Entity entity) {
	BackyardEnemy& enemy = registry.backyardEnemies.get(entity);
	if (!registry.hitTimers.has(entity)) {
		printf("MADE HIM DEAD\n");
		registry.hitTimers.emplace(entity);
		SpriteSheet& spriteSheet = registry.spriteSheets.get(enemy.imageOverlay);
		spriteSheet.dimensions = vec2(1,1);
		RenderRequest& rRequest = registry.renderRequests.get(enemy.imageOverlay);
		rRequest.used_texture = Textures::SQUIRREL_HIT;
	}
	enemy.health -= 1;
	if (enemy.health == 1) {
		AiController& controller = registry.aiControllers.get(entity);
		controller.stateMachine.enterState(EnemyStates::Return);
		RenderRequest& renderRequest = registry.renderRequests.get(enemy.imageOverlay);
        renderRequest.iUniforms["u_EnableHardcore"] = 1;

	}
	if (enemy.health <= 0) {
		registry.remove_all_components_of(enemy.imageOverlay); // delete image
		registry.remove_all_components_of(entity); // delete mesh
		incrementScore();
	}
}

void BackyardGameSystem::displayEndMsg(bool isWin) {


}

void BackyardGameSystem::displayUI(bool isDisplay) {
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


void BackyardGameSystem::endGame() {
	isRunning = false;
	displayUI(true);
	registry.remove_all_components_of(gun);
	registry.remove_all_components_of(station);
	registry.remove_all_components_of(scoreEntity);
	registry.remove_all_components_of(timerEntity);
	registry.remove_all_components_of(instructionEntity);
	registry.remove_all_components_of(ammo1);
	registry.remove_all_components_of(ammo2);

	for (Entity enemy : registry.backyardEnemies.entities) {
		BackyardEnemy& backyardEnemy = registry.backyardEnemies.get(enemy);
		registry.remove_all_components_of(backyardEnemy.imageOverlay);
		registry.remove_all_components_of(enemy);
	}
	updateStatusBars();

	worldSystem->changeRoom(2, 0);
}

void BackyardGameSystem::setupBackyardGame(Room *gameRoom) {
    room = gameRoom;
}
    

void BackyardGameSystem::stopInstructions() {
	isShowingInstructions = false;
	registry.remove_all_components_of(instructionEntity);
}

void BackyardGameSystem::playerInput(vec2 mouseMove) {
	mousePosition = mouseMove;
}

void BackyardGameSystem::updateStatusBars() { // todo: balance these updates
	buddyStatusSystem->entertainment += score / 2; 
	buddyStatusSystem->hygiene -= score / 3; 
}

void BackyardGameSystem::shoot() {
	if (registry.projectiles.components.size() < MAX_PROJECTILES) {
		if (registry.motions.has(ammo1) && registry.motions.has(ammo2)) {
			if (lastBullet) {
				Motion& ammo1Motion = registry.motions.get(ammo1);
				ammo1Motion.scale = vec2(0,0);
				noBullet = true;
				lastBullet = false;
			} else {
				Motion& ammo2Motion = registry.motions.get(ammo2);
				ammo2Motion.scale = vec2(0,0);
				lastBullet = true;
			}
		}
		// Motion gunMotion = registry.motions.get(gun);
		Motion gunMotion = registry.motions.get(station); // bullets come from center for now
		gunMotion.angle = atan2(mousePosition.y - gunMotion.position.y, mousePosition.x - gunMotion.position.x);
		Entity projectile = create_sprite_entity(renderer, gunMotion.position, vec2(30, 30), Textures::BULLET, false, -1);
		registry.projectiles.emplace(projectile);

		Motion& projectile_motion = registry.motions.get(projectile);
		projectile_motion.velocity = {350 * cos(gunMotion.angle), 350 * sin(gunMotion.angle)};

		Boundary& boundary = registry.boundaries.get(projectile);
		boundary.minBoundary = roomC - roomS / 2.f;
		boundary.maxBoundary = roomC + roomS / 2.f;
	}
}

void BackyardGameSystem::incrementScore() {
	score += 20;
	std::stringstream ss;
	ss << "Score: " << score;
	std::cout << ss.str().c_str() << std::endl;
	registry.texts.get(scoreEntity).text = ss.str().c_str();
	registry.texts.get(scoreEntity).offset = vec2(50.f, 50.f);
}

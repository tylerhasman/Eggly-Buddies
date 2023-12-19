#include "dream_game_system.h"
#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <sstream>

Entity player;
Entity scoreEntity;
Entity instructionEntity;

void DreamGameSystem::step(float elapsed_ms) {
	if (isRunning && !gameOver) {
		auto& motion_container = registry.motions; // remove offscreen enemies
		for (int i = (int)motion_container.components.size() - 1; i >= 0; --i) {
			Motion& motion = motion_container.components[i];
			if (motion.position.x + abs(motion.scale.x) < roomMin.x) {
				if (registry.dreamEnemies.has(motion_container.entities[i])) // only remove if its an enemy
					registry.remove_all_components_of(motion_container.entities[i]);
			}
		}
		spawnRandomEnemy(elapsed_ms);
		checkEnemiesAvoided();
		jumpCheck(elapsed_ms);
		speedupGame(elapsed_ms);
		displayInstructions(elapsed_ms);
	} else if (!isRunning && gameOver) {
		endGame(elapsed_ms);
	}
}

void DreamGameSystem::displayInstructions(float elapsed_ms) {
	if (isShowingInstructions) {
		curr_instruction_timer -= elapsed_ms;
		if (curr_instruction_timer < 0) {
			stopInstructions();
		}
	}
}

void DreamGameSystem::stopInstructions() {
	isShowingInstructions = false;
	curr_instruction_timer = INSTRUCTION_TIMER;
	registry.remove_all_components_of(instructionEntity);
}

void DreamGameSystem::speedupGame(float elapsed_ms) {
	next_speedup -= elapsed_ms;
	if (next_speedup < 0.f) {
		next_speedup = SPEED_UP_DELAY; //reset timer
		gameSpeed += speedIncrement;
		std::cout << "SPEED UP!" << std::endl;
	}
}

void DreamGameSystem::applyGravity(float step_seconds) {
	Motion& pMotion = registry.motions.get(player);
	pMotion.velocity.y += g * step_seconds / 20.f;
	pMotion.position.y += pMotion.velocity.y * step_seconds / 50.f;
}

void DreamGameSystem::jumpCheck(float elapsed_ms) {

    const float groundY = playerY + 30.0f;

	Motion& pMotion = registry.motions.get(player);
	if (isJumping && pMotion.position.y <= groundY) { // above ground
		applyGravity(elapsed_ms);
	}
	else if (isJumping && pMotion.position.y > groundY) { //hit ground
		isJumping = false;
		pMotion.velocity = vec2(0.f, 0.f);
		pMotion.position.y = groundY;
		SpriteSheet& spriteSheet = registry.spriteSheets.get(player);
		spriteSheet.dimensions = vec2(2, 1);
		spriteSheet.current = vec2(0, 0);
		spriteSheet.frameSpeed = 0.5f;
		RenderRequest& guyRender = registry.renderRequests.get(player);

        buddyStatusSystem->getBuddyLook(BuddyActionType::Run, guyRender.used_texture, spriteSheet);
	}
}

void DreamGameSystem::checkEnemiesAvoided() {
	auto& dreamContainer = registry.dreamEnemies;
	for (int i = (int)dreamContainer.components.size() - 1; i >= 0; i--) {
		Entity enemy = dreamContainer.entities[i];
		Motion& motion = registry.motions.get(enemy);
		if (motion.position.x < enemyCheckpoint && (dreamContainer.get(enemy).value > 0)) {
			addToScore(dreamContainer.get(enemy).value);
			dreamContainer.get(enemy).value = 0.f;
		}
	}
}

void DreamGameSystem::beginGame() {
	rng = std::default_random_engine(std::random_device()());
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	std::cout << "Dream Game: Begin" << std::endl;

    vec2 buddySize = buddyStatusSystem->getBuddySize();

	player = create_sprite_entity(renderer, roomC, buddySize, Textures::GUY_RUN_SPRITESHEET, false, -1);

    RenderRequest& playerRenderRequest = registry.renderRequests.get(player);

	//registry.players.emplace(player);
	CollisionHandler& enemyCollision = registry.collisionHandlers.emplace(player);
	enemyCollision.onCollision = [&](Entity entity, Collision collision) {
		if (registry.dreamEnemies.has(collision.other_entity))
		{
			hitEnemy();
		}};
	
	SpriteSheet& spriteSheet = registry.spriteSheets.emplace(player);
	spriteSheet.dimensions = vec2(2, 1);
	spriteSheet.current = vec2(0, 0);
	spriteSheet.frameSpeed = 0.5f;

    buddyStatusSystem->getBuddyLook(BuddyActionType::Run, playerRenderRequest.used_texture, spriteSheet);
	
	playerX = roomMin.x + roomS.x / 6.f;
	playerY = roomMin.y + (roomS.y * 3.f / 4.2f);
	enemyCheckpoint = roomMin.x + (roomS.x / 6.f);
	currPlayerY = playerY;
	isRunning = true;
	gameOver = false;
	isJumping = false;
	isShowingInstructions = true;
	displayUI(false);

	scoreEntity = create_sprite_entity(renderer, vec2(playerX, playerY), vec2(0, 0), Textures::RABBIT1, false, -10);
	Text textComp = {
		"Score: 0",
		vec2(350.f, -490.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f};
	registry.texts.emplace(scoreEntity, textComp);

	instructionEntity = create_sprite_entity(renderer, vec2(playerX, playerY), vec2(0, 0), Textures::RABBIT1, false, -10);
	Text instrText = {
		"PRESS SPACEBAR TO JUMP",
		vec2(250.f, -460.f),
		true,
		vec3(1.0f),
		Fonts::Retro,
		0.5f };
	registry.texts.emplace(instructionEntity, instrText);

	score = 0.f;
	std::stringstream ss;
	ss << "Score: " << score;
	std::cout << ss.str().c_str() << std::endl;
	Motion& motion = registry.motions.get(player);
	motion.position.x = playerX;
	motion.position.y = playerY + 30.0f;
	Mix_HaltMusic(); //background music switch
	Mix_PlayMusic(worldSystem->dream_game_music, -1);
}

void DreamGameSystem::displayUI(bool isDisplay) {
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

void DreamGameSystem::endGame(float elapsed_ms) {
	fadeTimer -= elapsed_ms;

	if (fadeTimer < 0) {
		fadeTimer = 3000.f;
		for (RenderRequest& renderReq : registry.renderRequests.components) {
			renderReq.iUniforms["u_EnableFadeToBlack"] = 0;
			renderReq.fUniforms["u_FadeToBlackFactor"] = 0.f;
		}
		isRunning = false;
		gameOver = false;
		isJumping = false;
		gameSpeed = 1.f;
		next_speedup = SPEED_UP_DELAY;
		stopInstructions();
		reduceTirednessAddHunger();
		score = 0.f;
		/*RenderRequest& sceneRequest = registry.renderRequests.get(background); //reset parallax
		sceneRequest.fUniforms["u_ParallaxSpeed"] = 1.f;
		RenderRequest& groundRequest = registry.renderRequests.get(ground);
		groundRequest.fUniforms["u_ParallaxSpeed"] = 4.f;*/
		worldSystem->changeRoom(-1, 0);
		displayUI(true);
		registry.remove_all_components_of(player);
		registry.remove_all_components_of(scoreEntity);
		registry.remove_all_components_of(instructionEntity);
		Mix_HaltMusic(); //reset to original background music
		Mix_PlayMusic(worldSystem->background_music, -1);
	}
	else {
		for (RenderRequest& renderReq : registry.renderRequests.components) {
			renderReq.fUniforms["u_FadeToBlackFactor"] = 1.f - fadeTimer / 3000.f;
		}
	}
}

void DreamGameSystem::spawnRandomEnemy(float elapsed_ms) {
	next_enemy_spawn -= elapsed_ms * gameSpeed; //spawning new enemy, code from template
	if (registry.dreamEnemies.components.size() <= MAX_ENEMIES && next_enemy_spawn < 0.f) {
		next_enemy_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng)*(ENEMY_DELAY_MS / 2); //reset timer
		if (rand() % 2) { //generates either 0 or 1 randomly
			Entity entity = create_sprite_entity(renderer, vec2(roomS.x - 64, roomS.y * 3.f / 4.2f),
				vec2(80, 80), Textures::RABBIT_SPRITESHEET, false, -1); //spawn rabbit
			SpriteSheet spriteSheet;
			spriteSheet.dimensions = vec2(2, 1);
			spriteSheet.current = vec2(0, 0);
			spriteSheet.frameSpeed = 0.5f;
			registry.spriteSheets.emplace(entity, spriteSheet);

			DreamEnemy dreamEnemy = { RABBIT_VALUE };
			registry.dreamEnemies.insert(entity, dreamEnemy);
			Motion& motion = registry.motions.get(entity); //initial position, constant velocity
			motion.position =
				vec2(roomS.x + 100.f, playerY+22.f);
			motion.velocity = vec2(1.5f * rabbit_speed, 0.f) * gameSpeed;
		}
		else {
			Entity entity = create_sprite_entity(renderer, vec2(roomS.x - 64, roomS.y * 3.f / 4.2f),
				vec2(135, 100), Textures::SHEEP_SPRITESHEET, false, -1); //spawn sheep
			SpriteSheet spriteSheet;
			spriteSheet.dimensions = vec2(2, 1);
			spriteSheet.current = vec2(0, 0);
			spriteSheet.frameSpeed = 0.5f;
			registry.spriteSheets.emplace(entity, spriteSheet);

			DreamEnemy dreamEnemy = { SHEEP_VALUE };
			registry.dreamEnemies.insert(entity, dreamEnemy);
			Motion& motion = registry.motions.get(entity); //initial position, constant velocity
			motion.position =
				vec2(roomS.x + 100.f, playerY+20.f);
			motion.velocity = vec2(rabbit_speed, 0.f) * gameSpeed;
		}
	}
}

void DreamGameSystem::jump() {
	if (!isJumping && registry.motions.get(player).position.y >= playerY) {
		isJumping = true;
		registry.motions.get(player).velocity.y = -jumpVel;
		SpriteSheet& spriteSheet = registry.spriteSheets.get(player);
		spriteSheet.dimensions = vec2(1, 1);
		spriteSheet.current = vec2(0, 0);
		spriteSheet.frameSpeed = 0.f;
		RenderRequest& guyRender = registry.renderRequests.get(player);

        buddyStatusSystem->getBuddyLook(BuddyActionType::Jump, guyRender.used_texture, spriteSheet);

		Mix_PlayChannel(-1, worldSystem->jump_sound, 0);
	}
}

void DreamGameSystem::hitEnemy() {
	SpriteSheet& spriteSheet = registry.spriteSheets.get(player);
	spriteSheet.dimensions = vec2(1, 1);
	spriteSheet.current = vec2(0, 0);
	spriteSheet.frameSpeed = 0.f;
	RenderRequest& guyRender = registry.renderRequests.get(player);
    buddyStatusSystem->getBuddyLook(BuddyActionType::Dead, guyRender.used_texture, spriteSheet);

    Mix_PlayChannel(-1, worldSystem->death_sound, 0);

	std::stringstream ss;
	ss << "Final score: " << score;
	std::cout << ss.str().c_str() << std::endl;

	for (RenderRequest& renderReq : registry.renderRequests.components) {
		renderReq.iUniforms["u_EnableFadeToBlack"] = 1;
		renderReq.fUniforms["u_FadeToBlackFactor"] = 0.f;
	}
	isRunning = false;
	gameOver = true;

	for (Entity entity : registry.dreamEnemies.entities) {
		registry.remove_all_components_of(entity);
	}
	registry.texts.get(scoreEntity).text = ss.str().c_str();
	registry.texts.get(scoreEntity).offset = vec2(300.f, -250.f);
	std::cout << "Dream Game: End" << std::endl;
}
void DreamGameSystem::reduceTirednessAddHunger() {
	// buddyStatusSystem->tiredness -= score / MAX_TIREDNESS_REDUCTION * 100.f;
	// buddyStatusSystem->hunger += score / MAX_HUNGER_ADDITION * 100.f;
	buddyStatusSystem->rest += score / MAX_TIREDNESS_REDUCTION * 100.f;  //TODO: tune
	buddyStatusSystem->satiation -= score / MAX_HUNGER_ADDITION * 100.f; // TODO: tune
}

void DreamGameSystem::addToScore(float toAdd) {
	score += toAdd;
	std::stringstream ss;
	ss << "Score: " << score;
	std::cout << ss.str().c_str() << std::endl;
	registry.texts.get(scoreEntity).text = ss.str().c_str();
}

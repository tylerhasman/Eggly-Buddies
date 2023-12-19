#include "chatbox_system.h"
#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <sstream>

Entity currBox; //only one chatbox can be active at a time

void ChatboxSystem::onClickClose() {
	if (isBoxActive) {
		registry.remove_all_components_of(currBox);
		isBoxActive = false;
	}
}

void ChatboxSystem::createBox(std::string text, vec2 offset, float scale) {
	if (!isBoxActive) {
		currBox = create_sprite_entity(renderer, pos, size, Textures::CHATBOX_SPRITESHEET, true, -1);
		SpriteSheet& spriteSheet = registry.spriteSheets.emplace(currBox); //animate
		spriteSheet.dimensions = vec2(2, 1);
		spriteSheet.current = vec2(0, 0);
		spriteSheet.frameSpeed = 0.5f;
		Text textComp = {
		text,
		offset,
		true,
		vec3(1.0f),
		Fonts::Retro,
		scale };
		registry.texts.emplace(currBox, textComp);
		Clickable clickable;
		clickable.onClick = [&](Entity entity) {
			onClickClose();
			};
		registry.clickables.emplace(currBox, clickable);
		isBoxActive = true;
	}
}

bool ChatboxSystem::returnIsBoxActive() {
	return isBoxActive;
}
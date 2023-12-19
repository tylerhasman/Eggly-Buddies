#include "intro_system.h"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/ai/buddy_state_machine.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Entity egg;
bool eggExists = false;
Entity chatBox;
void IntroSystem::showIntroEvent() {
    introStep = IntroStep::IntroInProgress;
    if (!eggExists) {
        std::cout << "no egg, spawning one" << std::endl;
        egg = create_sprite_entity(renderer, vec2(game_width / 2.f, game_height + 650), vec2(44, 39) * 5.f, Textures::EGG_IDLE, false, -1);
        SpriteSheet spriteSheet;
        spriteSheet.dimensions = vec2(2, 1);
        spriteSheet.current = vec2(0, 0);
        spriteSheet.frameSpeed = 0.5f;
        registry.spriteSheets.emplace(egg, spriteSheet);
        eggExists = true;
    }
    Clickable clickable;
    clickable.onClick = [&](Entity entity) {
        onIntroEventClicked(entity, introIndex);
        };

    vec2 size = vec2(131, 32) * 5.f;
    vec2 pos = vec2(game_width / 2.0f, game_height / 2.0f - 200);
    chatBox = create_sprite_entity(renderer, pos, size, Textures::CHATBOX_SPRITESHEET, true, -1);
    SpriteSheet& spriteSheet = registry.spriteSheets.emplace(chatBox); //animate
    spriteSheet.dimensions = vec2(2, 1);
    spriteSheet.current = vec2(0, 0);
    spriteSheet.frameSpeed = 0.5f;
    registry.mailEvents.emplace(chatBox);


    string filename = std::string(PROJECT_SOURCE_DIR) + "data/introEvent.txt";
    string line = readLineAtIndex(filename, introIndex);

    if (!line.empty()) {
        //std::cout << "Line at index " << introIndex << ": " << line << std::endl;
    }
    Text introText = { line, vec2(-275, -10), true, vec3(1.0f),
        Fonts::Retro, 0.5f, true };
    registry.texts.emplace(chatBox, introText);
    registry.clickables.emplace(chatBox, clickable);

}

void IntroSystem::onIntroEventClicked(Entity entity, int index) {
    if (index >= MAX_INTRO_INDEX) {
        registry.remove_all_components_of(chatBox);
        Clickable clickable;
        clickable.onClick = [&](Entity entity) {
            registry.clickables.remove(egg);
            SpriteSheet& spriteSheet = registry.spriteSheets.get(egg);
            spriteSheet.dimensions = vec2(4, 1);
            spriteSheet.current = vec2(0, 0);
            spriteSheet.frameSpeed = 0.2f;
            spriteSheet.looping = false;
            RenderRequest& eggRender = registry.renderRequests.get(egg);
            eggRender.used_texture = Textures::EGG_HATCH;
            Clickable clickFunc;
            clickFunc.onClick = [&](Entity entity) {
                for (Entity other : registry.mailEvents.entities) {
                    registry.mailEvents.remove(other);
                    registry.remove_all_components_of(other);
                }
                introStep = IntroStep::IntroFinished;
                displayUI(true);
                registry.remove_all_components_of(egg);
                eggExists = false;
                registry.aiControllers.get(registry.players.entities[0]).stateMachine.enterState(BuddyState::States::Idle);
                registry.motions.get(registry.players.entities[0]).position.x = game_width / 2.0f;
                };
            registry.clickables.emplace(egg, clickFunc);
            };
        registry.clickables.emplace(egg, clickable);
    }
    else {
        registry.aiControllers.get(registry.players.entities[0]).stateMachine.enterState(BuddyState::States::NoAi);
        registry.remove_all_components_of(chatBox);
        introIndex++;
        showIntroEvent();
    }
}

void IntroSystem::displayUI(bool isDisplay) {
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

std::string IntroSystem::readLineAtIndex(const std::string& filename, int index) {
    std::ifstream newfile(filename, std::ios::in);
    if (!newfile.is_open()) {
        std::cerr << "Error opening the file: " << filename << std::endl;
        return ""; // error
    }

    std::string line;
    for (int currentLine = 0; currentLine <= index; ++currentLine) {
        if (!std::getline(newfile, line)) {
            std::cerr << "Error reading line at index " << index << "." << std::endl;
            newfile.close();
            return ""; // error
        }
    }

    newfile.close();
    return line;
}
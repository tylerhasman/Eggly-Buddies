#include "end_system.h"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/ai/buddy_state_machine.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Entity endChatBox;
Entity theEnd;
void EndSystem::showEndEvent(GLFWwindow* window) {
    Clickable clickable;
    clickable.onClick = [&](Entity entity) {
        onEndEventClicked(entity, endIndex, window);
        };

    vec2 size = vec2(131, 32) * 5.f;
    vec2 pos = vec2(roomC.x, roomC.y - 200);
    endChatBox = create_sprite_entity(renderer, pos, size, Textures::CHATBOX_SPRITESHEET, true, -1);
    SpriteSheet& spriteSheet = registry.spriteSheets.emplace(endChatBox); //animate
    spriteSheet.dimensions = vec2(2, 1);
    spriteSheet.current = vec2(0, 0);
    spriteSheet.frameSpeed = 0.5f;
    registry.mailEvents.emplace(endChatBox);


    string filename = std::string(PROJECT_SOURCE_DIR) + "data/endEvent.txt";
    string line = readLineAtIndex(filename, endIndex);

    if (!line.empty()) {
        //std::cout << "Line at index " << introIndex << ": " << line << std::endl;
    }
    Text endText = { line, vec2(-275, -10), true, vec3(1.0f),
        Fonts::Retro, 0.5f, true };
    registry.texts.emplace(endChatBox, endText);
    registry.clickables.emplace(endChatBox, clickable);

}

void EndSystem::onEndEventClicked(Entity entity, int index, GLFWwindow* window) {
    if (index >= MAX_END_INDEX) {
        registry.remove_all_components_of(endChatBox);
        theEnd = create_sprite_entity(renderer, roomC, vec2(0, 0), Textures::CHATBOX_SPRITESHEET, true, -1);
        Text endText = { "THE END", vec2(60, -30), true, vec3(1.0f),
        Fonts::Retro, 0.9f, true };
        registry.texts.emplace(theEnd, endText);
        // Destroy music components
        /*if (worldSystem->background_music != nullptr)
            Mix_FreeMusic(worldSystem->background_music);
        if (worldSystem->dream_game_music != nullptr)
            Mix_FreeMusic(worldSystem->dream_game_music);
        if (worldSystem->ui_click_sound != nullptr)
            Mix_FreeChunk(worldSystem->ui_click_sound);
        if (worldSystem->jump_sound != nullptr)
            Mix_FreeChunk(worldSystem->jump_sound);
        if (worldSystem->death_sound != nullptr)
            Mix_FreeChunk(worldSystem->death_sound);
        Mix_CloseAudio();

        for (auto room : worldSystem->rooms) {
            delete room;
        }

        // Destroy all created components
        registry.clear_all_components();

        // Close the window
        glfwDestroyWindow(window);*/
    }
    else {
        //registry.aiControllers.get(registry.players.entities[0]).stateMachine.enterState(BuddyState::States::NoAi);
        registry.remove_all_components_of(endChatBox);
        endIndex++;
        showEndEvent(window);
    }
}

void EndSystem::displayUI(bool isDisplay) {
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

std::string EndSystem::readLineAtIndex(const std::string& filename, int index) {
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
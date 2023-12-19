#include "mail_system.h"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void MailSystem::step(float elapsed_ms) {

    if (mailState == MailState::TimerTicking && nextMailEventSpawn <= 0.f) {
        const float minimumTimer = 10000.0; //reset timer
        const float maximumTimer = 30000.0;
        const float randomTime = random(rng) * (maximumTimer - minimumTimer) + minimumTimer;
        Clickable clickable;
        clickable.onClick = [&](Entity entity){
            onMailEventClicked(entity);
        };
        nextMailEventSpawn = randomTime;
        std::cout << "Mail event timer reset to: " << randomTime << " milliseconds" << std::endl;
        int zToUse = registry.renderRequests.get(registry.userInterfaces.entities[0]).z_index - 1; //for if ui is paused in dream minigame
        Entity mail_icon_notif = create_sprite_entity(renderer, vec2(game_width - 64, 64), vec2(128, 128), Textures::MAIL_NOTIF_ICON, false, zToUse);
        registry.userInterfaces.emplace(mail_icon_notif);
        registry.mailEvents.emplace(mail_icon_notif);
        registry.clickables.emplace(mail_icon_notif, clickable);
        mailState = MailState::NotificationWaiting; //awaiting user input
    }
    else if (mailState == MailState::TimerTicking) {
        // std::cout << "mailTimer: " << nextMailEventSpawn << std::endl;
        nextMailEventSpawn -= elapsed_ms;
    }
}

int MailSystem::showRandomMailEvent() {
    int randomIndex = std::rand() % NUM_MAIL_EVENTS;
    vec2 size = vec2(game_width, game_height);
    vec2 pos = vec2(game_width / 2.0f, game_height / 2.0f);
    Entity postcard = create_sprite_entity(renderer, pos, size, Textures::POSTCARD, true,-1);
    registry.mailEvents.emplace(postcard);
    string filename = std::string(PROJECT_SOURCE_DIR) + "data/mailEvent.txt";
    string line = readLineAtIndex(filename, randomIndex);

    if (!line.empty()) {
        //std::cout << "Line at index " << introIndex << ": " << line << std::endl;
    }
    Text mailText = { line, vec2(-140, 30), true, vec3(0.0f), Fonts::Retro, 0.5f, true};
    registry.texts.emplace(postcard , mailText);
    Entity closeButton = create_sprite_entity(renderer,
                                              vec2((game_width * 4.3f / 5.0f), (game_height / 5.8f)),
                                              vec2(game_width / 12.0f, game_width / 12.0f), Textures::CLOSE, true, -2);
    registry.mailEvents.emplace(closeButton);
    Clickable clickable;
    clickable.onClick = [&](Entity entity) {
        onMailEventClicked(entity);
        };
    registry.clickables.emplace(closeButton, clickable);
    return randomIndex;
}

void MailSystem::displayUI(bool isDisplay) {
	if (!isDisplay) {
		for (Entity entity : registry.userInterfaces.entities) {
			RenderRequest& renderReq = registry.renderRequests.get(entity);
			renderReq.z_index += tempUIZIncrement;
		}
	} else {
		for (Entity entity : registry.userInterfaces.entities) {
			RenderRequest& renderReq = registry.renderRequests.get(entity);
			renderReq.z_index -= tempUIZIncrement;
		}
	}
}

void MailSystem::onMailEventClicked(Entity entity) {
    if (mailState == MailState::NotificationWaiting) { //show postcard
        recentMailIndex = showRandomMailEvent();
        mailState = MailState::PostcardOnScreen; //postcard is on screen
    } else if (mailState == MailState::PostcardOnScreen) { //close the postcard
        mailState = MailState::TimerTicking; //resume timer
        for (Entity other : registry.mailEvents.entities) {
            registry.remove_all_components_of(other);
        }
        giveReward(recentMailIndex);
    }
}

void MailSystem::giveReward(int recentIndex) {
    float minReward = -20.f;
    float maxReward = 20.f;
    float value = minReward + static_cast<float>(std::rand()) / RAND_MAX * (maxReward - minReward);
    switch (getQuadrant(recentIndex)) 
    {
    case 1:
        buddyStatusSystem->entertainment += value;
        std::cout << "entertainment affected by mail event by: " << value << std::endl;
        return;
    case 2:
        buddyStatusSystem->hygiene += value;
        std::cout << "hygiene affected by mail event by: " << value << std::endl;
        return;
    case 3:
        buddyStatusSystem->rest += value;
        std::cout << "rest affected by mail event by: " << value << std::endl;
        return;
    case 4:
        buddyStatusSystem->satiation += value;
        std::cout << "satiation affected by mail event by: " << value << std::endl;
        return;
    }
}

int MailSystem::getQuadrant(int index) {
    if (index < 0 || index >= NUM_MAIL_EVENTS) { //invalid, shouldnt happen
        return -1;
    }
    int quadrantSize = NUM_MAIL_EVENTS / 4;
    if (index < quadrantSize) {
        return 1;
    } else if (index < 2 * quadrantSize) {
        return 2;
    } else if (index < 3 * quadrantSize) {
        return 3;
    } else {
        return 4;
    }
}


std::string MailSystem::readLineAtIndex(const std::string& filename, int index) {
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
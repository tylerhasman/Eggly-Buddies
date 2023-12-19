#pragma once

#include <random>
#include "systems/core/world_system.hpp"

enum class MailState {
    TimerTicking = 0,
    NotificationWaiting = 1,
    PostcardOnScreen = 2,
    Pause = 3,
    Count
};

class MailSystem {

public:

    MailSystem(RenderSystem* renderer) : renderer(renderer) {

    }

    void step(float elapsed_ms);
    void giveReward(int recentIndex);
    std::string readLineAtIndex(const std::string& filename, int index);
    MailState mailState = MailState::TimerTicking;

private:
    std::default_random_engine rng;
    std::uniform_real_distribution<float> random;

    const int NUM_MAIL_EVENTS = 23;

    float nextMailEventSpawn = 0.0f;

    RenderSystem* renderer;

    int showRandomMailEvent();
    int getQuadrant(int index);

    void onMailEventClicked(Entity entity);

    void displayUI(bool isDisplay);
    int tempUIZIncrement = 200;

    int recentMailIndex = 0;

};

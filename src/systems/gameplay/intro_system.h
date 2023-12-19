#pragma once

#include <random>
#include "systems/core/world_system.hpp"

enum class IntroStep {
    IntroNotRunning = 0,
    IntroStarted = 1,
    IntroInProgress = 2,
    IntroFinished = 3,

    Count
};

class IntroSystem {

public:

    IntroSystem(RenderSystem* renderer) : renderer(renderer) {

    }

    void showIntroEvent();
    std::string readLineAtIndex(const std::string& filename, int index);

    IntroStep introStep = IntroStep::IntroStarted;

private:

    // index of postcard currently being shown
    int introIndex = 0;
    const int MAX_INTRO_INDEX = 12;

    RenderSystem* renderer;

    void onIntroEventClicked(Entity entity, int index);

    void displayUI(bool isDisplay);
    int tempUIZIncrement = 200;

};

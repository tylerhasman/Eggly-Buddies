#pragma once

#include <random>
#include "systems/core/world_system.hpp"
#include "systems/rooms/end_room.h"

class EndSystem {

public:

    EndSystem(RenderSystem* renderer) : renderer(renderer) {

    }

    void showEndEvent(GLFWwindow* window);

    vec2 roomC = vec2(0.f, 0.f);

private:
    int endIndex = 0;
    const int MAX_END_INDEX = 10;

    RenderSystem* renderer;

    void onEndEventClicked(Entity entity, int index, GLFWwindow* window);
    std::string readLineAtIndex(const std::string& filename, int index);

    void displayUI(bool isDisplay);
    int tempUIZIncrement = 200;

};
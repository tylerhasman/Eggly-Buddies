#pragma once

#include <random>
#include "systems/core/world_system.hpp"

class ChatboxSystem {

public:
    ChatboxSystem(RenderSystem* renderer) : renderer(renderer) {

    }

    void onClickClose();
    void createBox(std::string text, vec2 offset = vec2(-275.f, -10.f), float scale = 0.5f);
    bool returnIsBoxActive();
    WorldSystem* worldSystem = nullptr;

private:
    RenderSystem* renderer;
    bool isBoxActive = false;
    vec2 pos = vec2(game_width / 2.0f, game_height / 2.0f - 200);
    vec2 size = vec2(131, 32) * 5.f;
};

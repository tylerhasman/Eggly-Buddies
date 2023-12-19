#pragma once

#include "engine/common.hpp"
#include "engine/tiny_ecs.hpp"
#include "systems/core/render_system.hpp"

Entity createSalmon(RenderSystem* renderer, vec2 pos);

Entity createSquirrel(RenderSystem* renderer, vec2 pos);

Entity createSquirrelImage(RenderSystem* renderer, vec2 pos);

Entity create_sprite_entity(RenderSystem* renderer, vec2 position, vec2 scale, Textures texture, bool isUI, int z = 0);

Entity create_pickupable(RenderSystem* renderer, vec2 position, vec2 scale, Textures texture);

Entity create_particles(RenderSystem* renderer, vec2 position, vec2 scale, Textures texture, int z = -3);


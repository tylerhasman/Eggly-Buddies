#include <random>
#include "world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/pickup_system.h"

Entity createSalmon(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 150.f;
	motion.scale.x *= -1; // point front to the right

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ Textures::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			Effects::COLOURED,
			GEOMETRY_BUFFER_ID::SALMON });

	return entity;
}

Entity createSquirrel(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

    // create_sprite_entity(renderer, pos, vec2(64,72), Textures::SQUIRREL_1, false, -1);

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SQUIRREL);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 25.f;
	// motion.scale = vec2(64, 72);

	motion.scale.x *= 1; // point front to the right
	motion.scale.y *= -1; // flip on y axis
    motion.angle += M_PI / 2;

    // motion.angle += M_PI;



	// Create and (empty) Salmon component to be able to refer to all turtles
	// registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ Textures::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			Effects::COLOURED,
			GEOMETRY_BUFFER_ID::SQUIRREL });
    
    // SpriteSheet spriteSheet;
	// 		spriteSheet.dimensions = vec2(2, 1);
	// 		spriteSheet.current = vec2(0, 0);
	// 		spriteSheet.frameSpeed = 0.5f;
	// 		registry.spriteSheets.emplace(entity, spriteSheet);

	return entity;
}

Entity createSquirrelImage(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

    // create_sprite_entity(renderer, pos, vec2(64,72), Textures::SQUIRREL_1, false, -1);

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	// motion.scale = mesh.original_size * 25.f;
	motion.scale = vec2(60, 85);

	motion.scale *= -1; // flip on all axis
    motion.angle += M_PI;



	// Create and (empty) Salmon component to be able to refer to all turtles
	// registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ Textures::SQUIRREL_SPRITESHEET, // TEXTURE_COUNT indicates that no txture is needed
			Effects::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
            -2 }); // DEPTH INFRONT OF MESHES
    
    SpriteSheet spriteSheet;
			spriteSheet.dimensions = vec2(2, 1);
			spriteSheet.current = vec2(0, 0);
			spriteSheet.frameSpeed = 0.5f;
			registry.spriteSheets.emplace(entity, spriteSheet);

	return entity;
}

Entity create_sprite_entity(RenderSystem* renderer, vec2 position, vec2 scale, Textures texture, bool isUI, int z) {
    // Reserve an entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.position = position;

    motion.scale = scale;

    registry.boundaries.emplace(entity);

    if(isUI){
        z -= 100;
    }

    RenderRequest request = {texture,
                             Effects::TEXTURED,
                             GEOMETRY_BUFFER_ID::SPRITE,
                             z
    };

    registry.renderRequests.insert(entity,request);

    if(isUI){
        registry.userInterfaces.emplace(entity);
    }

    return entity;
}

Entity create_pickupable(RenderSystem* renderer, vec2 position, vec2 scale, Textures texture) {
    Entity entity = create_sprite_entity(renderer, position, scale, texture, false);

    Clickable clickable;
    clickable.onClick = PickupSystem::onPickupClicked;

    registry.clickables.emplace(entity, clickable);
    registry.pickupables.emplace(entity);
    Boundary& boundary = registry.boundaries.get(entity);
    boundary.minBoundary = {40.f, 20.f};
    boundary.maxBoundary = {window_width_px - 40.f, window_height_px - 20.f}; 

    return entity;
}

Entity create_particles(RenderSystem* renderer, vec2 position, vec2 scale, Textures texture, int z) {
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the position, scale, and physics components
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.position = position;

    motion.scale = scale;

    registry.boundaries.emplace(entity);

    RenderRequest request = {texture,
                             Effects::PARTICLES,
                             GEOMETRY_BUFFER_ID::SPRITE,
                             z
    };

    registry.renderRequests.insert(
            entity,
            request);

    return entity;
}

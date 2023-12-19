#include <iostream>
// internal
#include "physics_system.hpp"
#include "engine/world_init.hpp"
#include "engine/mesh_collision.h"
#include "systems/gameplay/gameplay_systems.h"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

bool pointInside(vec2 point, vec2 bbPosition, vec2 bbSize){
	if(point.x > bbPosition.x - bbSize.x / 2 && point.x < bbPosition.x + bbSize.x / 2){
		if(point.y > bbPosition.y - bbSize.y / 2 && point.y < bbPosition.y + bbSize.y / 2){
			return true;
        }
    }

	return false;
}

bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 onePosition = motion1.position;
	vec2 twoPosition = motion2.position;

	vec2 oneSize = motion1.scale;
	vec2 twoSize = motion2.scale;

	bool oneInsideOfTwo = pointInside(onePosition, twoPosition, twoSize) 
		|| pointInside(onePosition + vec2(oneSize.x / 2.0f, 0.0), twoPosition, twoSize)
		|| pointInside(onePosition + vec2(0.0, oneSize.y / 2.0f), twoPosition, twoSize)
		|| pointInside(onePosition + oneSize / 2.0f, twoPosition, twoSize);

	bool twoInsideOfOne = pointInside(twoPosition, onePosition, oneSize) 
		|| pointInside(twoPosition + vec2(twoSize.x / 2.0f, 0.0), onePosition, oneSize)
		|| pointInside(twoPosition + vec2(0.0, twoSize.y / 2.0f), onePosition, oneSize)
		|| pointInside(twoPosition + twoSize / 2.0f, onePosition, oneSize);

	return oneInsideOfTwo || twoInsideOfOne;
}

void PhysicsSystem::step(float elapsed_ms)
{

    if(elapsed_ms > maxPhysicsStep){
        elapsed_ms = maxPhysicsStep;
    }

	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_container = registry.motions;
	for(uint i = 0; i < motion_container.size(); i++)
	{
		Motion& motion = motion_container.components[i];
		Entity entity = motion_container.entities[i];
		Boundary boundary;

        if(registry.boundaries.has(entity)){
            boundary = registry.boundaries.get(entity);
        }

		float step_seconds = elapsed_ms / 1000.f;

		vec2 bounding_box = get_bounding_box(motion);

        motion.lastPhysicsPosition = motion.position;

		if (registry.players.has(entity)) {

            if (motion.position.x + motion.velocity.x * step_seconds > boundary.maxBoundary.x - bounding_box.x / 2.f) {
				motion.position.x = boundary.maxBoundary.x - bounding_box.x / 2.f;
			}
			else if (motion.position.x + motion.velocity.x * step_seconds < boundary.minBoundary.x + bounding_box.x / 2.f) {
				motion.position.x = bounding_box.x / 2.f;
			} else {
				motion.position.x += motion.velocity.x * step_seconds;
			}

            if(registry.players.get(entity).enableGravity){
                motion.velocity.y += 2000.0f * elapsed_ms / 1000.0f;

                motion.position += motion.velocity * elapsed_ms / 1000.0f;

                if(motion.position.y > game_height / 2.0f + 256.0f){
                    motion.velocity.y = 0.0f;
                    motion.position.y = game_height / 2.0f + 256.0f;
                }
            }

        } else if (registry.pickupables.has(entity)) {

                if(motion.position.x < boundary.minBoundary.x){
                    motion.velocity.x = -motion.velocity.x * 0.9f;
                    motion.position.x = boundary.minBoundary.x;
                }else if(motion.position.x > boundary.maxBoundary.x){
                    motion.velocity.x = -motion.velocity.x * 0.9f;
                    motion.position.x = boundary.maxBoundary.x;
                }

                if(motion.position.y < boundary.minBoundary.y){
                    motion.velocity.y = -motion.velocity.y * 0.5f;
                    motion.position.y = boundary.minBoundary.y;
                }else if(motion.position.y > boundary.maxBoundary.y){
                    motion.velocity.y = -motion.velocity.y * 0.5f;
                    motion.position.y = boundary.maxBoundary.y;

                    if(glm::abs(motion.velocity.y) < 100.0f){
                        motion.velocity.y = 0.0f;
                    }
                }

                if(glm::abs(motion.velocity.x) < 0.1f){
                    motion.velocity.x = 0.0f;
                }

				motion.position.x += motion.velocity.x * elapsed_ms;
				motion.position.y += motion.velocity.y * (elapsed_ms / 1000);

                if(motion.position.y < boundary.maxBoundary.y){
                    motion.velocity.y += 2000.0f * elapsed_ms / 1000.0f;
                }

                motion.velocity.x *= (1.0f - elapsed_ms / 1000.0f);

        } else if (registry.projectiles.has(entity)) {
			if (motion.position.x + motion.velocity.x * step_seconds > boundary.maxBoundary.x 
				|| 	motion.position.y + motion.velocity.y * step_seconds > boundary.maxBoundary.y
				||	motion.position.x + motion.velocity.x * step_seconds < boundary.minBoundary.x
				||	motion.position.y + motion.velocity.y * step_seconds < boundary.minBoundary.y) { 
					registry.remove_all_components_of(entity);
					if (registry.motions.has(backyardGameSystem->ammo1) && registry.motions.has(backyardGameSystem->ammo2)) {
						if (backyardGameSystem->noBullet) {
							Motion& ammo1Motion = registry.motions.get(backyardGameSystem->ammo1);
							ammo1Motion.scale = vec2(16,16);
							backyardGameSystem->noBullet = false;
							backyardGameSystem->lastBullet = true;
						} else if (backyardGameSystem->lastBullet) {
							Motion& ammo2Motion = registry.motions.get(backyardGameSystem->ammo2);
							ammo2Motion.scale = vec2(16,16);
							backyardGameSystem->lastBullet = false;
						}
					}
			} else {
				motion.position.x += motion.velocity.x * step_seconds;
				motion.position.y += motion.velocity.y * step_seconds;
			}

		} else {
			motion.position.x += motion.velocity.x * step_seconds;
			motion.position.y += motion.velocity.y * step_seconds;
		}

	}

	// Check for collisions between all moving entities
	for(uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		const float entitySize = glm::max(motion_i.scale.x, motion_i.scale.y) / 2.0f;

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			Entity entity_j = motion_container.entities[j];

            if(motion_j.enableMeshCollision || motion_i.enableMeshCollision){
                continue;
            }

            if (collides(motion_i, motion_j))
			{
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}

		Circle circle = {
			motion_i.position,
			entitySize
		};

		Triangle triangle = {};

		for(Entity other : registry.meshPtrs.entities){

            if(other == entity_i){
                continue;
            }

			Motion& otherMotion = registry.motions.get(other);
			Mesh* mesh = registry.meshPtrs.get(other);

            if(!otherMotion.enableMeshCollision){
                continue;
            }

            //vec2 totalNormal = vec2(0.0);
            //loat hits = 0.0f;

			for(uint16_t j = 0; j < mesh->vertex_indices.size();j += 3){

				int indexOne = mesh->vertex_indices[j];
				int indexTwo = mesh->vertex_indices[j+1];
				int indexThree = mesh->vertex_indices[j+2];

				ColoredVertex& one = mesh->vertices[indexOne];
				ColoredVertex& two = mesh->vertices[indexTwo];
				ColoredVertex& three = mesh->vertices[indexThree];

				triangle.p1 = vec2(one.position) * otherMotion.scale + otherMotion.position;
				triangle.p2 = vec2(two.position) * otherMotion.scale + otherMotion.position;
				triangle.p3 = vec2(three.position) * otherMotion.scale + otherMotion.position;

				vec2 normal;

				if(MeshCollision::CircleTriangleCollision(circle, triangle, normal)){
					//We can use the normal later to resolve collisions properly

                    registry.collisions.emplace_with_duplicates(entity_i, other, normal);
                    registry.collisions.emplace_with_duplicates(other, entity_i, normal);

                    break;

                    //TODO: Doesnt work well yet but this is the proper way to do this
                    /*hits++;
                    totalNormal += normal;*/
				}

			}

            // Calculate response normal by adding taking the average normal hit
            //TODO: Doesnt work well yet but this is the proper way to do this
            /*if(hits > 0){
                totalNormal /= hits;
                registry.collisions.emplace_with_duplicates(entity_i, other, totalNormal);
                registry.collisions.emplace_with_duplicates(other, entity_i, totalNormal);
            }*/

		}

	}

}

//
// Created by Tyler on 2023-10-29.
//

#include "animation_system.h"
#include "engine/tiny_ecs_registry.hpp"

void AnimationSystem::step(float elapsed_ms) {
    for(SpriteSheet& spriteSheet : registry.spriteSheets.components){

        if(spriteSheet.frameSpeed > 0.0f){
            spriteSheet.nextFrame += elapsed_ms / 1000.0f;

            if(spriteSheet.nextFrame >= spriteSheet.frameSpeed){

                spriteSheet.nextFrame -= spriteSheet.frameSpeed;

                spriteSheet.current.x++;

                if(spriteSheet.current.x >= spriteSheet.dimensions.x){

                    if(spriteSheet.current.y + 1 >= spriteSheet.dimensions.y){
                        if (spriteSheet.looping) {
                            spriteSheet.current.x = 0;
                            spriteSheet.current.y = 0;
                        } else {
                            spriteSheet.current.x--;
                            spriteSheet.frameSpeed = 0.0f;
                        }
                    } else {
                        spriteSheet.current.x = 0;
                        spriteSheet.current.y++;
                    }
                }
            }
        }

        // assume we'll never stop on frame 0,0
        // if (stopOn[0] > 0 && stopOn[1] > 0) {
        //     stopOnFrame(stopOn[0], stopOn[1], spriteSheet);
        // }

    }
}

// void AnimationSystem::stopOnFrame(int image_index_x, int image_index_y, SpriteSheet spriteSheet) {
//     if (spriteSheet.current.x == image_index_x && spriteSheet.current.y == image_index_y) {
//         spriteSheet.frameSpeed = 0.0f;
//     }
// }

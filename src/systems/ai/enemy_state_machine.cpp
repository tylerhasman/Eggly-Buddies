#include "enemy_state_machine.h"
#include "../gameplay/gameplay_systems.h"


EnemyIdleState::EnemyIdleState(const Entity &enemy) : EnemyState(enemy) {

}

int EnemyIdleState::step(float elapsed_ms) {

    BackyardEnemy& backyardEnemy = getBackyardEnemy();
    if (registry.projectiles.has(backyardEnemy.currentProjectile)) {
        Boundary currProjectileBoundary = registry.boundaries.get(backyardEnemy.currentProjectile);
        Motion currProjectileMotion = registry.motions.get(backyardEnemy.currentProjectile);
        if (reachedBoundary(currProjectileMotion.position, currProjectileBoundary)) {
            backyardEnemy.isDodging = false;
            return EnemyStates::Return;
        } else {
            return -1;
        }
    }

    return EnemyStates::Return;
}

void EnemyIdleState::onStateEntered() {
    Motion& enemyMotion = getEnemyMotion();
    enemyMotion.velocity = vec2(0,0);
    updateImageOverlay();

}

EnemyMovingState::EnemyMovingState(const Entity &enemy, vec2 roomC, vec2 roomS, float initialAngle) : EnemyState(enemy)  {
    centerPosition = roomC;
    getBackyardEnemy().angle = initialAngle;
}

int EnemyMovingState::step(float elapsed_ms) {
    Motion& enemyMotion = getEnemyMotion();
    BackyardEnemy& enemyEnt = getBackyardEnemy();
    calculateHardCorePosition();

    if (backyardGameSystem->backyardLevel <= 3) {
        enemyMotion.position.x = centerPosition.x + radiusAroundBuddy * cos(enemyEnt.angle);
        enemyMotion.position.y = centerPosition.y + radiusAroundBuddy * sin(enemyEnt.angle);
        updateImageOverlay();
        enemyEnt.pathPosition = enemyMotion.position;
        enemyEnt.angle += angle_speed * (elapsed_ms / 1000.0f);
        return EnemyStates::Moving;
    }

     // todo: chekc if can block anyhting, and get ready to block/ go to block state with same movements. (maybe threshold before moving to block)
    for(uint i = 0; i < registry.backyardEnemies.components.size(); i++) {
        Entity backyardAlly = registry.backyardEnemies.entities[i];
        Motion allyMotion = registry.motions.get(backyardAlly);
        BackyardEnemy& ally = registry.backyardEnemies.get(backyardAlly);
        BackyardEnemy& blocker = getBackyardEnemy();
        if (ally.health == 1 && blocker.health > 1) { // IF ON 1 SHOT AND YOU CAN TANK
            if (ally.isDodging) {
                for(uint j = 0; j < registry.projectiles.components.size(); j++) {
                    Entity projectile = registry.projectiles.entities[j];
                    Motion projectileMotion = registry.motions.get(projectile);
                    Boundary boundary = registry.boundaries.get(projectile);

                    if (willIntersectStill(projectileMotion, boundary, allyMotion, 500, 50)) {
                        if (distanceBetween(enemyMotion.position, allyMotion.position) <= 500.f) {
                            blocker.blockPosition = getBlockPosition(allyMotion.position);
                            blocker.ally = backyardAlly;
                            return EnemyStates::Block;
                        }
                        
                    }
                }
                
            }
        }
    }

     for(uint i = 0; i < registry.backyardEnemies.components.size(); i++) {
        Entity& backyardAlly = registry.backyardEnemies.entities[i];
        Motion allyMotion = registry.motions.get(backyardAlly);
        BackyardEnemy& ally = registry.backyardEnemies.get(backyardAlly);

        if (backyardAlly != enemy && enemyEnt.health == ally.health) {
            if (distanceBetween(allyMotion.position, enemyMotion.position) <= 100.f && enemyEnt.angle < ally.angle && !ally.isDodging) {
                enemyEnt.ally = backyardAlly;
                return EnemyStates::Backoff;
            }
        }
    }

    for(uint i = 0; i < registry.projectiles.components.size(); i++) {
        Entity projectile = registry.projectiles.entities[i];
        Motion projectileMotion = registry.motions.get(projectile);
        Boundary boundary = registry.boundaries.get(projectile);
            float distToReact = 50;
           
            if (willIntersect(projectileMotion, boundary, centerPosition, radiusAroundBuddy, angle_speed, enemyEnt.angle, 500, distToReact)) {
                double dist = distanceBetween(projectileMotion.position, enemyMotion.position);
                if (dist < dodgeRadius) {
                    enemyEnt.currentProjectile = projectile;
                    enemyEnt.pathPosition = enemyMotion.position;
                    return EnemyStates::Dodge;
                }
            }
    }

    enemyMotion.position.x = centerPosition.x + radiusAroundBuddy * cos(enemyEnt.angle);
    enemyMotion.position.y = centerPosition.y + radiusAroundBuddy * sin(enemyEnt.angle);

    updateImageOverlay();

    enemyEnt.pathPosition = enemyMotion.position;

    enemyEnt.angle += angle_speed * (elapsed_ms / 1000.0f);

    return EnemyStates::Moving;
}

void EnemyMovingState::onStateEntered() {
    if (getBackyardEnemy().health == 1) {
        radiusAroundBuddy = 350.f;
    }
}

EnemyDodgeState::EnemyDodgeState(const Entity &enemy) : EnemyState(enemy) {
    
}

int EnemyDodgeState::step(float elapsed_ms) {

    BackyardEnemy& backyardEnemy = getBackyardEnemy();

    if (registry.projectiles.has(backyardEnemy.currentProjectile)) {
            Motion currProjectileMotion = registry.motions.get(backyardEnemy.currentProjectile);
            if (distanceBetween(currProjectileMotion.position, getEnemyMotion().position) > 150.f) {
                backyardEnemy.dodgePosition = getEnemyMotion().position;
                backyardEnemy.isDodging = true;
                return EnemyStates::Idle;
            } else {
                return -1;
            }
    }
    return EnemyStates::Return;
}

void EnemyDodgeState::onStateEntered() {
    Motion& enemyMotion = getEnemyMotion();
    BackyardEnemy backyardEnemy = getBackyardEnemy();
    if (registry.projectiles.has(backyardEnemy.currentProjectile)) {
        Motion currProjectileMotion = registry.motions.get(backyardEnemy.currentProjectile);

        vec2 dodgeDirection = getDodgeDirection(currProjectileMotion.velocity);

        enemyMotion.velocity.x = dodgeDirection.x * 1.2f;
        enemyMotion.velocity.y = dodgeDirection.y * 1.2f;

        updateImageOverlay();

    }
}

EnemyBlockState::EnemyBlockState(const Entity &enemy) : EnemyState(enemy) {

}

int EnemyBlockState::step(float elapsed_ms) {
    Motion& enemyMotion = getEnemyMotion();
    BackyardEnemy backyardEnemy = getBackyardEnemy();
    vec2 returnPosition = backyardEnemy.blockPosition;

    if (registry.backyardEnemies.has(backyardEnemy.ally)) {
        BackyardEnemy backyardAlly = registry.backyardEnemies.get(backyardEnemy.ally);

        if (glm::abs(enemyMotion.position.x - returnPosition.x) >= returnThreshold) {
            if (enemyMotion.position.x < returnPosition.x) {
                enemyMotion.position.x += blockSpeed * elapsed_ms / 1000.0f;
            } else {
                enemyMotion.position.x -= blockSpeed * elapsed_ms / 1000.0f;
            }
        }

        if (glm::abs(enemyMotion.position.y - returnPosition.y) >= returnThreshold) {
            if (enemyMotion.position.y < returnPosition.y) {
                enemyMotion.position.y += blockSpeed * elapsed_ms / 1000.0f;
            } else {
                enemyMotion.position.y -= blockSpeed * elapsed_ms / 1000.0f;
            }
        }

        updateImageOverlay();


        if (!backyardAlly.isDodging) {
            return EnemyStates::Return;
        }

        if (glm::abs(enemyMotion.position.x - returnPosition.x) < returnThreshold && glm::abs(enemyMotion.position.y - returnPosition.y) < returnThreshold) {
            return EnemyStates::Block;
        }

    } else {
        return EnemyStates::Return;
    }
    

    return -1;
}

void EnemyBlockState::onStateEntered() {

}

EnemyReturnState::EnemyReturnState(const Entity &enemy) : EnemyState(enemy) {
    
}

int EnemyReturnState::step(float elapsed_ms) {
    Motion& enemyMotion = getEnemyMotion();\
    vec2 returnPosition;

    if (getBackyardEnemy().health == 1) {
        returnPosition = getHardCorePathPosition();
    } else {
        returnPosition = getPathPosition();
    }

    if (glm::abs(enemyMotion.position.x - returnPosition.x) >= returnThreshold) {
        if (enemyMotion.position.x < returnPosition.x) {
            enemyMotion.position.x += returnSpeed * elapsed_ms / 1000.0f;
        } else {
            enemyMotion.position.x -= returnSpeed * elapsed_ms / 1000.0f;
        }
    }

    if (glm::abs(enemyMotion.position.y - returnPosition.y) >= returnThreshold) {
        if (enemyMotion.position.y < returnPosition.y) {
            enemyMotion.position.y += returnSpeed * elapsed_ms / 1000.0f;
        } else {
            enemyMotion.position.y -= returnSpeed * elapsed_ms / 1000.0f;
        }
    }

    updateImageOverlay();

    if (glm::abs(enemyMotion.position.x - returnPosition.x) < returnThreshold && glm::abs(enemyMotion.position.y - returnPosition.y) < returnThreshold) {
        getBackyardEnemy().aboutToDodge = false;
        return EnemyStates::Moving;
    }

    return -1;
}

void EnemyReturnState::onStateEntered() {
    Motion& enemyMotion = getEnemyMotion();
    getBackyardEnemy().aboutToDodge = true;

    enemyMotion.velocity = vec2(0,0);
    updateImageOverlay();

}

EnemyHitState::EnemyHitState(const Entity &enemy) : EnemyState(enemy) {
    
}

int EnemyHitState::step(float elapsed_ms) {

    return -1;
}

void EnemyHitState::onStateEntered() {

}

EnemyBackoffState::EnemyBackoffState(const Entity &enemy, vec2 roomC) : EnemyState(enemy) {
    centerPosition = roomC;
}

int EnemyBackoffState::step(float elapsed_ms) {

    BackyardEnemy& backyardEnemy = getBackyardEnemy();
    Motion& enemyMotion = getEnemyMotion();
    Entity& ally = backyardEnemy.ally;

    Motion allyMotion = registry.motions.get(ally);

    enemyMotion.position.x = centerPosition.x + radiusAroundBuddy * cos(backyardEnemy.angle);
    enemyMotion.position.y = centerPosition.y + radiusAroundBuddy * sin(backyardEnemy.angle);

    if (distanceBetween(enemyMotion.position, allyMotion.position) >= 200.f) {
        return EnemyStates::Moving;
    }

    backyardEnemy.pathPosition = enemyMotion.position;

    updateImageOverlay();

    backyardEnemy.angle -= angle_speed * (elapsed_ms / 1000.0f);



    return EnemyStates::Backoff;
}

void EnemyBackoffState::onStateEntered() {
    if (getBackyardEnemy().health == 1) {
        radiusAroundBuddy = 350.f;
    }
}

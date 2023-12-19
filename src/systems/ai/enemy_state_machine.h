#pragma once

#include "state_machine.h"
#include "engine/tiny_ecs.hpp"
#include "engine/tiny_ecs_registry.hpp"

enum EnemyStates : int {
    Idle,
    Moving,
    Dodge,
    Block,
    Return,
    Hit,
    Backoff,
    Count
};

class EnemyState : public State {

public:
    explicit EnemyState(Entity enemy) : enemy(enemy){

    }

protected:
    Entity enemy;

    // DOESNT WORK CAUSE CANT KEEP TRACK OF CURRENT PROJECTILE CAUSE STATES DONT SHARE FIELDS
    // Entity currentProjectile;

    float dodgeRadius = 100.f;

    float returnThreshold = 5.f;

    float angle_speed = 0.5f;

    float radiusAroundBuddy = 250.f;

    // float angle;

    // vec2 roomC = roomC;

    // vec2 roomS = roomS;
    
    vec2 centerPosition;

    Motion& getEnemyMotion(){
        return registry.motions.get(enemy);
    }

    BackyardEnemy& getBackyardEnemy(){
        return registry.backyardEnemies.get(enemy);
    }

    // DOESNT WORK CAUSE CANT KEEP TRACK OF CURRENT PROJECTILE CAUSE ECS
    // Motion& getProjectileMotion() {
    //     return registry.motions.get(currentProjectile);
    // }

    vec2 getPathPosition() {
        return getBackyardEnemy().pathPosition;
    }

    vec2 getHardCorePathPosition() {
        return getBackyardEnemy().hardCorePathPosition;
    }

    vec2 getDodgeDirection(vec2 direction) {
        return vec2(direction.y, -direction.x); // Simple 90-degree counterclockwise rotation
    }

    double distanceBetween(vec2 pos1, vec2 pos2) {
        return (std::sqrt(std::pow(pos2.x - pos1.x, 2) + std::pow(pos2.y - pos1.y, 2)));
    }

    bool reachedBoundary(vec2 position, Boundary boundary) {
        return position.x < boundary.minBoundary.x + 100 ||
                position.x > boundary.maxBoundary.x - 100 ||
                position.y < boundary.minBoundary.y + 100 ||
                position.y > boundary.maxBoundary.y - 100;
    }

    // Boundary getBoundary(vec2 roomC, vec2 roomS) {
    //     Boundary boundary;
    //     boundary.maxBoundary.x = roomC.x + roomS.x / 2;
    //     boundary.maxBoundary.y = roomC.y + roomS.y / 2;
    //     boundary.minBoundary.x = roomC.x - roomS.x / 2;
    //     boundary.minBoundary.y = roomC.y - roomS.y / 2;
    //     return boundary;
    // }

    bool willIntersect(Motion projectileMotion, Boundary projectileBoundary,
                       vec2 simcenterPosition, float simradiusAroundBuddy, float simangle_speed, float simangle, int numSteps, float distToReact) {
        vec2 projectileFuturePosition = projectileMotion.position;
        vec2 enemyFuturePosition = simcenterPosition;

        for (int i = 0; i < numSteps; ++i) {
            
            // Update projectile position
            projectileFuturePosition.x += projectileMotion.velocity.x / 10;
            projectileFuturePosition.y += projectileMotion.velocity.y / 10;

            // CHECK IF PROJECTILE REACHED BOUNDARY
            if (reachedBoundary(projectileFuturePosition, projectileBoundary)) {
                return false;
            }

            // Update enemy position
            simangle += simangle_speed / 10;
            enemyFuturePosition.x = simcenterPosition.x + simradiusAroundBuddy * cos(simangle);
            enemyFuturePosition.y = simcenterPosition.y + simradiusAroundBuddy * sin(simangle);

            // Check if object and enemy will intersect in the future
            double dist = distanceBetween(projectileFuturePosition, enemyFuturePosition);
            if (dist <= distToReact) {
                return true; 
            }
        }

        return false; // Projectile reached boundary
    }

    bool willIntersectStill(Motion projectileMotion, Boundary projectileBoundary,
                       Motion stillMotion, int numSteps, float distToReact) {
        vec2 projectileFuturePosition = projectileMotion.position;

        for (int i = 0; i < numSteps; ++i) {
            
            // Update projectile position
            projectileFuturePosition.x += projectileMotion.velocity.x / 10;
            projectileFuturePosition.y += projectileMotion.velocity.y / 10;

            // CHECK IF PROJECTILE REACHED BOUNDARY
            if (reachedBoundary(projectileFuturePosition, projectileBoundary)) {
                return false;
            }

            // Check if object and enemy will intersect in the future
            double dist = distanceBetween(projectileFuturePosition, stillMotion.position);
            if (dist <= distToReact) {
                return true; 
            }
        }

        return false; // Projectile reached boundary
    }

    void updateImageOverlay() {
        Entity& image = getBackyardEnemy().imageOverlay;
        Motion & imageMotion = registry.motions.get(image);
        imageMotion.position = getEnemyMotion().position;
        imageMotion.velocity = getEnemyMotion().velocity;
    }

};

class EnemyIdleState : public EnemyState {

public:
    explicit EnemyIdleState(const Entity &enemy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;
private:
    Entity focusedProjectile;
    
};


class EnemyMovingState : public EnemyState {
public:
    explicit EnemyMovingState(const Entity &enemy, vec2 roomC, vec2 roomS, float initialAngle);

    int step(float elapsed_ms) override;

    void onStateEntered() override;

private:
    

    void calculateHardCorePosition() {
        getBackyardEnemy().hardCorePathPosition.x = centerPosition.x + (350.f) * cos(getBackyardEnemy().angle);
        getBackyardEnemy().hardCorePathPosition.y = centerPosition.y + (350.f) * sin(getBackyardEnemy().angle);
    }

    vec2 getBlockPosition(vec2 position) {
        return vec2(((centerPosition.x - position.x) * 1 / 5) + position.x, ((centerPosition.y - position.y) * 1 / 5) + position.y);
    }
};

class EnemyDodgeState : public EnemyState {
public:
    explicit EnemyDodgeState(const Entity &enemy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;

private:
};

class EnemyBlockState : public EnemyState {
public:
    explicit EnemyBlockState(const Entity &enemy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;
private:
    float blockSpeed = 250.f;
};

class EnemyReturnState : public EnemyState {
public:
    explicit EnemyReturnState(const Entity &enemy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;

private:
    float returnSpeed = 200.f;    
};

class EnemyHitState : public EnemyState {

public:
    explicit EnemyHitState(const Entity &enemy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;
private:
   
};

class EnemyBackoffState : public EnemyState {

public:
    explicit EnemyBackoffState(const Entity &enemy, vec2 roomC);

    int step(float elapsed_ms) override;

    void onStateEntered() override;
private:
   
};

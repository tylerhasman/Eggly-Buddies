//
// Created by Tyler on 2023-11-19.
//

#pragma once

#include "state_machine.h"
#include "engine/tiny_ecs.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/buddy_status_system.h"


enum class BuddyDirection {
    Left,
    Right
};

class BuddyState : public State {

public:
    explicit BuddyState(Entity buddy) : buddy(buddy){

    }

    enum States : int {
        Idle,
        WanderRandom,
        FindRandomPickup,
        FollowPlayer,
        NoAi,
        Evolve,
        Poo,
        Dead,

        Count
    };

    virtual BuddyActionType getBuddyAction() const {
        return BuddyActionType::Dead;
    }

    virtual BuddyDirection getBuddyDirection() const {
        return BuddyDirection::Right;
    }

    bool isBuddyOnGround() const {
        Motion& buddyMotion = getBuddyMotion();

        return buddyMotion.position.y >= game_height / 2.0f + 256.0f && glm::abs(buddyMotion.velocity.y) < 0.1f;
    }

protected:
    Entity buddy;

    Motion& getBuddyMotion() const {
        return registry.motions.get(buddy);
    }

};

class BuddyDeathState : public BuddyState {
public:
    explicit BuddyDeathState(const Entity& buddy) : BuddyState(buddy) {

    }

    int step(float elapsed_ms) override;

    void onStateEntered() override;

    BuddyDirection getBuddyDirection() const override{
        return BuddyDirection::Left;
    }

    BuddyActionType getBuddyAction() const override {
        return BuddyActionType::Dead;
    }

};

class BuddyEvolveState : public BuddyState {
public:
    explicit BuddyEvolveState(const Entity& buddy) : BuddyState(buddy) {

    }

    int step(float elapsed_ms) override;

    void onStateEntered() override;

    BuddyDirection getBuddyDirection() const override;

    BuddyActionType getBuddyAction() const override;

private:
    int jumpsLeft = 3;

};

class BuddyIdleState : public BuddyState {

public:
    explicit BuddyIdleState(const Entity &buddy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;

    BuddyActionType getBuddyAction() const override {
        return BuddyActionType::Idle;
    }

    BuddyDirection getBuddyDirection() const override {
        return direction;
    }

private:
    float idleDuration = 0;

    float jumpTime = 0.0f;

    BuddyDirection direction = BuddyDirection::Left;

};

class BuddyWanderRandomState : public BuddyState {
public:
    explicit BuddyWanderRandomState(const Entity &buddy);

    int step(float elapsed_ms) override;

    void onStateEntered() override;

    BuddyActionType getBuddyAction() const override {
        return BuddyActionType::Run;
    }

    BuddyDirection getBuddyDirection() const override;

private:
    vec2 selectedPosition;

    float jumpTime;
};

class BuddyNoAi : public BuddyState {
public:
    explicit BuddyNoAi(const Entity &buddy);

    int step(float elapsed_ms) override {
        return -1;
    }

    BuddyActionType getBuddyAction() const override {
        return BuddyActionType::Sleep;
    }

    BuddyDirection getBuddyDirection() const override{
        return BuddyDirection::Right;
    }

    void onStateEntered() override {

    }
};

class BuddyFollowPlayer : public BuddyState {
public:
    explicit BuddyFollowPlayer(const Entity &buddy);

    int step(float elapsed_ms) override;

    BuddyActionType getBuddyAction() const override {
        return BuddyActionType::Run;
    }

    void onStateEntered() override;
    BuddyDirection getBuddyDirection() const override;
};


class BuddyPoo: public BuddyState {
public:
    explicit BuddyPoo(const Entity &buddy);
    int step(float elapsed_ms) override;
    void onStateEntered() override;

    BuddyDirection getBuddyDirection() const override {
        return BuddyDirection::Left;
    }

    BuddyActionType getBuddyAction() const override {
        return BuddyActionType::Run;
    }

private:
    std::pair<vec2, vec2> bathroomPosition;
    vec2 bathroomDest;

};

//
// Created by Tyler on 2023-11-19.
//

#pragma once

#include <unordered_map>
#include <iostream>
#include <random>

class State {

public:
    virtual ~State() = default;

    // Step this state, return -1 to remain in current state, or another states id to transition to it
    virtual int step(float elapsed_ms) = 0;

    virtual void onStateEntered() = 0;

protected:
    float randomFloat(){
        return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

};

class StateMachine {

public:
    StateMachine(){
        currentState = nullptr;
    }

    ~StateMachine(){
        states.clear();
        currentState = nullptr;
    }

    StateMachine(StateMachine&& other) noexcept
            : states(std::move(other.states)), currentState(other.currentState) {

        other.currentState = nullptr;
    }

    StateMachine& operator=(StateMachine const & other)  noexcept = default;

    void addState(int stateId, const std::shared_ptr<State>& state);

    void enterState(int stateId);

    void step(float elapsed_ms);

    std::shared_ptr<State> getCurrentState() {
        return currentState;
    }

private:
    std::shared_ptr<State> currentState;

    std::unordered_map<int, std::shared_ptr<State>> states;
};

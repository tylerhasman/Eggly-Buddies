//
// Created by Tyler on 2023-11-19.
//

#include "state_machine.h"

void StateMachine::addState(int stateId, const std::shared_ptr<State>& state) {
    if(state == nullptr){
        std::cerr << "Can't add null state!" << std::endl;
        return;
    }
    if(stateId < 0){
        std::cerr << "Couldn't add state " << stateId << ", state id must be zero or greater!" << std::endl;
        return;
    }
    states[stateId] = state;
}

void StateMachine::enterState(int stateId) {
    if (states.find(stateId) == states.end()) {
        std::cout << "Couldn't enter state " << stateId << ", it doesn't exist!" << std::endl;
        return;
    }
    currentState = states[stateId];
    currentState->onStateEntered();
}

void StateMachine::step(float elapsed_ms) {
    if(currentState){
        int newState = currentState->step(elapsed_ms);

        if(newState >= 0){
            enterState(newState);
        }
    }
}

#pragma once

#include "systems/core/world_system.hpp"
// #include "systems/rooms/bathroom_game_room.h"
#include "systems/rooms/bath_room.h"
#include <random>
#include <map>


enum BathRoomGameItems:int
{
    Poop,
    CleanTool,
};

class BathroomGameSystem {

public:
    bool isRunning = false;
    bool userPickTime = false;
    BathroomGameSystem(RenderSystem* renderer) : renderer(renderer) {}
    
    void step(float elapsed_ms);
    void setUpSystem(Room* GameRoom);
    void beginGame();
    void displayEndMsg(bool wrongPick);
    void endGame();
    void putPoopAnimation();
    void onhitBook();
    void prizeDrop();
    void getBathroom(BathRoom* room);





private:
    RenderSystem* renderer;
    Room* room;
    BathRoom* bathroom;
    Entity book_left;
    Entity book_middle;
    Entity book_right;
    Entity fallingPoop;
    vec2 book1StartPos;
    vec2 book2StartPos;
    Entity selectedBook1;
    Entity selectedBook2;
    Entity displayText;
    Entity instructionEntity;
    Entity roundEntity;
    
    bool isSwapping = false;

    int roundNum; // 3 rounds to win 
    float  swapDurationTime = 2.f; // 2/roundNum seconds to swap

    bool wrongPick = false;


    
    bool shuffleTime  = false;
    bool fallingTime = false;

    float shuffleAccumulatedTime = 0.0f;
    float shuffleStartTime = 0.0f;
    float shuffleInterval;
    
    float displayMessageTimer = 0.0f;
    float instructionTimer = 0.f;
    float roundTimer = 0.f;


    int totalSwaps;          // Total number of swaps to perform
    int swapsDone;           // Number of swaps already done

    vec2 leftPos;
    vec2 middlePos;
    vec2 rightPos;
    int tempUIZIncrement = 200;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist;
    void updateBookPositions(float elapsed_ms_since_start_of_shuffle, float movementDuration);
    vec2 calculateArcCenter(vec2 positionA, vec2 positionB, bool upper);
    float lerp(float a, float b, float t);
    void startShuffle();
    float randomDuration(float minDuration, float maxDuration);
    void chooseBooksToSwap();
    void swapTwoBooks(Entity book1, Entity book2, float elapsed_ms, float movementDuration);
    void moveBookOnArc(Motion& motion, vec2 startPos, vec2 endPos, vec2 arcCenter, float t, bool upper);
    void onBookClicked(Entity book);
    void displayUI(bool isDisplay);

};



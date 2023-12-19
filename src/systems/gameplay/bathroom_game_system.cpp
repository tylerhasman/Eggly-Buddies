#include "systems/core/world_system.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/world_init.hpp"
#include "engine/tiny_ecs_registry.hpp"
#include "systems/gameplay/gameplay_systems.h"
#include "systems/rooms/bathroom_game_room.h"
#include "systems/rooms/bath_room.h"
#include "systems/gameplay/inventory_system.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include "systems/rooms/backyard.h"


void BathroomGameSystem::step(float elapsed_ms) {
    // game process here 
 
    // If the accumulated time is less than the shuffle interval, update positions
	if (shuffleTime) {
      
		shuffleAccumulatedTime += elapsed_ms;
		float movementDuration = 2000.f; 
		// If the accumulated time is less than the shuffle interval, update positions
        updateBookPositions(shuffleAccumulatedTime, movementDuration);
		
    } else if (userPickTime){
        // std::cout << "user pick time\n";
        Clickable clickable;
        clickable.onClick = [&](Entity entity){
            onBookClicked(entity);
        };
    } else if (fallingTime) {
        // std::cout << "falling time\n";
        putPoopAnimation();
        auto& motion = registry.motions.get(fallingPoop);
        motion.velocity.y += 100.f*roundNum;
        if(motion.position.y > room->roomCenter[1]){
            onhitBook();
        }
    }

    displayMessageTimer -= elapsed_ms / 1000.f;
    if (displayMessageTimer < 0.f) {
        registry.remove_all_components_of(displayText);
    }

    instructionTimer -= elapsed_ms / 1000.f;
    if (instructionTimer < 0.f) {
        registry.remove_all_components_of(instructionEntity);
    }

    roundTimer -= elapsed_ms / 1000.f;
    if (roundTimer < 0.f) {
        registry.remove_all_components_of(roundEntity);
    }
}

void BathroomGameSystem::setUpSystem(Room* GameRoom) {
    room = GameRoom;
}

void BathroomGameSystem::getBathroom(BathRoom* myBathroom) {
    bathroom = myBathroom;
}

void BathroomGameSystem::startShuffle() {
    totalSwaps = std::rand() % 12 + 1; // Random number of total swaps, for example 1 to 3
    swapsDone = 0;                      // Reset the counter for swaps done
    chooseBooksToSwap();
    shuffleAccumulatedTime = 0.0f;      // Reset the shuffle timer
    isSwapping = true;
    shuffleTime = true;
}

void BathroomGameSystem::onBookClicked(Entity entity) {
    std::cout << "book clicked\n";
    if(entity == book_middle){
        // std::cout << "book_middle\n";
        if(roundNum == 3){
            endGame();
        } else {
            roundNum++;
            shuffleTime = false;
            userPickTime = false; 
            fallingTime = true; 
            isSwapping = true;
            swapsDone = 0;

            // remove book from clickable 
            for(Entity& book: registry.books.entities){
                    // std::cout << "book: " << book << "\n";
                    registry.clickables.remove(book);
                }
            
            registry.renderRequests.remove(book_left);
            registry.renderRequests.remove(book_middle);
            registry.renderRequests.remove(book_right);
        }
    }
    else {
        wrongPick = true;
        endGame();
    }
}

void BathroomGameSystem::updateBookPositions(float elapsed_ms_since_start_of_shuffle, float movementDuration) {
    movementDuration /= roundNum;
    if (isSwapping && swapsDone < totalSwaps) {
        swapTwoBooks(selectedBook1, selectedBook2, elapsed_ms_since_start_of_shuffle, movementDuration);
        if (elapsed_ms_since_start_of_shuffle >= movementDuration) {
            // Swap complete, prepare for next swap if needed
            swapsDone++;
            if (swapsDone < totalSwaps) {
                chooseBooksToSwap(); // Choose new books
                shuffleAccumulatedTime = 0.0f; // Reset timer
            }
            else {
                isSwapping = false; // No more swaps needed
                shuffleTime = false;
                userPickTime = true;
                shuffleAccumulatedTime = 0.0f; // Reset timer
                
                Clickable clickable;
                clickable.onClick = [&](Entity entity){
                    onBookClicked(entity);
                };
                
                for(Entity& book: registry.books.entities){
                    // std::cout << "book: " << book << "\n";
                    registry.clickables.emplace(book,clickable);
                }
            }
        }
    }
}


void BathroomGameSystem::chooseBooksToSwap() {
    // Randomly choose two different books to swap and store their start positions
    std::vector<Entity> books = { book_left, book_middle, book_right };
    std::random_shuffle(books.begin(), books.end()); // Shuffle the vector
    selectedBook1 = books[0];
    selectedBook2 = books[1];

    // Store their starting positions
    book1StartPos = registry.motions.get(selectedBook1).position;
    book2StartPos = registry.motions.get(selectedBook2).position;
}


void BathroomGameSystem::swapTwoBooks(Entity book1, Entity book2, float elapsed_ms, float movementDuration) {



    // Retrieve motion components for both books
    auto& motion1 = registry.motions.get(book1);
    auto& motion2 = registry.motions.get(book2);

    // // Calculate the centers for the upper and lower semi-circles
    vec2 upperCenter = calculateArcCenter(book1StartPos, book2StartPos, true);
    vec2 lowerCenter = calculateArcCenter(book1StartPos, book2StartPos, false);

    // std::cout << "uppercenter: " << upperCenter.x << ", " << upperCenter.y << "\n";
    // std::cout << "lowercenter: " << lowerCenter.x << ", " << lowerCenter.y << "\n";
    // Determine the completion ratio of the swap

    float t = elapsed_ms / movementDuration;


    // Swap book1 along the upper semi-circle
    moveBookOnArc(motion1, book1StartPos, book2StartPos, upperCenter, t, true);

    // Swap book2 along the lower semi-circle
    moveBookOnArc(motion2, book2StartPos, book1StartPos, lowerCenter, t, false);

    // const double PI = 3.14159265358979323846;
    // float duriation = swapDurationTime/roundNum;
    // vec2 centerPoint ((motion1.position.x - motion2.position.x) / 2, motion1.position.y);
    // float radius = std::abs(motion1.position.x - motion2.position.x) / 2;
    // double angle =  std::fmin(elapsed_ms / duriation, 1.0) * PI;

    // Update positions
    // motion1.position.x = static_cast<float>(centerPoint.x + radius * cos(angle));
    // motion1.position.y = static_cast<float>( centerPoint.y + radius * sin(angle));

    // motion2.position.x = static_cast<float>(centerPoint.x + radius * cos(angle));
    // motion2.position.y = static_cast<float>(centerPoint.y - radius * sin(angle));
    
}

// Moves a book along an arc
void BathroomGameSystem::moveBookOnArc(Motion& motion, vec2 startPos, vec2 endPos, vec2 arcCenter, float t, bool upper) {
    float radius = glm::distance(startPos, arcCenter);
    float startAngle = atan2(startPos.y - arcCenter.y, startPos.x - arcCenter.x);
    float endAngle = atan2(endPos.y - arcCenter.y, endPos.x - arcCenter.x);

    
    // Adjust the end angle for a semi-circular path
    if (upper) {
        if (endAngle < startAngle) endAngle += 2*M_PI;
    }
    else {
        if (endAngle > startAngle) endAngle -= 2*M_PI;
    }

   
    //t = std::max(0.0f, std::min(t, 1.0f));
    float angle = lerp(startAngle, endAngle, t);

    vec2 newPosition = vec2{ arcCenter.x + radius * cos(angle), arcCenter.y + radius * sin(angle) };
    float minX = room->roomMin.x; // Replace with actual minimum x boundary
    float maxX = room->roomMax.x; // Replace with actual window width
    float minY = room->roomMin.y + 100; // Replace with actual minimum y boundary
    float maxY = room->roomMax.y - 100; // Replace with actual window height

    newPosition.x = std::max(minX, std::min(newPosition.x, maxX));
    newPosition.y = std::max(minY, std::min(newPosition.y, maxY));

    motion.position = newPosition;
}

vec2 BathroomGameSystem::calculateArcCenter(vec2 positionA, vec2 positionB, bool upper) {
    // Midpoint between start and end points
    vec2 midpoint = { (positionA.x + positionB.x) / 2.0f, (positionA.y + positionB.y) / 2.0f };

    // Calculate the distance between the two points
    float distance = glm::distance(positionA, positionB);
    float radius = distance / 2.f;

   //arc center
    vec2 center = midpoint;

    center.y += upper ? -radius : radius;

    return center;
}


float BathroomGameSystem::lerp(float a, float b, float t) {
    return a + t * (b - a);
}


void BathroomGameSystem::beginGame() {
    // std::cout << "begin bath room game\n";
    isRunning = true;
    putPoopAnimation();
    roundNum = 1;
    wrongPick = false;
    displayUI(false);
    instructionEntity = create_sprite_entity(renderer, vec2(room->roomMin.x,  room->roomMin.y), vec2(0, 0), Textures::SOAP, false, -10);
	Text textComp = {
		"Pick the box with the soap!",
		// vec2(350.f, -490.f),
        vec2{300.f,100.f},
		true,
		vec3(7.0f),
		Fonts::Retro,
		0.7f};
	registry.texts.emplace(instructionEntity, textComp);

    instructionTimer = 5.f;
}



void BathroomGameSystem::endGame() {
    for(const Entity& book : registry.books.entities){
        registry.remove_all_components_of(book);
    };
    
    displayEndMsg(wrongPick);
	worldSystem->changeRoom(-2, 0);
    prizeDrop();
    
    wrongPick = false;
    roundNum = 1;
    isRunning = false;
    displayUI(true);
}

// void BathroomGameSystem::displayEndMsg(bool isWin) {}

void BathroomGameSystem::prizeDrop() {
    if(!wrongPick) {
        Entity soap = bathroom->spawnItem(vec2{bathroom->roomCenter[0]+300,bathroom->roomCenter[1]}, vec2(60*1.3, 60), Textures::SOAP);
        auto &soap_render = registry.renderRequests.get(soap);
        soap_render.z_index = -10;
        registry.cleanTools.emplace(soap);
        // if(uniform_dist(rng)<0.5){
        //     bathroom->spawnItem(vec2{bathroom->roomCenter[0]+180,bathroom->roomCenter[1]}, vec2(40, 60), Textures::SHAMPOO);
        // } else {
        //     bathroom->spawnItem(vec2{bathroom->roomCenter[0]+120,bathroom->roomCenter[1]}, vec2(40, 60), Textures::BODYWASH);
        // }

    // set wrongPick back to false

    }
    Inventory::adjustItemAmount(InventoryItemType::Fertilizer, 1);

    Backyard::updateFertilizerText();
    
    // TODO: drop fertilizer no matter WIN OR LOSE ???
}



void BathroomGameSystem::putPoopAnimation() {

    roundEntity = create_sprite_entity(renderer, vec2(room->roomMin.x,  room->roomMin.y), vec2(0, 0), Textures::SOAP, false, -10);
	
    Text textComp; 

    if (roundNum ==2 ) {
        textComp = {
		"ROUND 2!",
		// vec2(350.f, -490.f),
        vec2{400.f,200.f},
		true,
        vec3{1.0f, 0.5f, 0.0f},
		Fonts::Retro,
		1.f};
    } else if (roundNum == 3) {
        textComp =  {
		"ROUND 3!",
		// vec2(350.f, -490.f),
        vec2{400.f,200.f},
		true,
        vec3{1.0f, 0.0f, 0.0f},
		Fonts::Retro,
		1.f};
    } else {
        textComp = {
		" ",
		// vec2(350.f, -490.f),
        vec2{400.f,200.f},
		true,
        vec3{1.f},
		Fonts::Retro,
		1.f};
    };

	registry.texts.emplace(roundEntity, textComp);

    roundTimer = 2.f;



    Entity openedBook_left = room->spawnEntity(vec2{room->roomCenter[0]-175,room->roomCenter[1]}, vec2(120, 120), Textures::GIFTBOX_OPEN, 1);
    Entity openedBook_middle = room->spawnEntity(vec2{room->roomCenter[0],room->roomCenter[1]}, vec2(120, 120), Textures::GIFTBOX_OPEN, 1);
    Entity openedBook_right = room->spawnEntity(vec2{room->roomCenter[0]+175,room->roomCenter[1]}, vec2(120, 120), Textures::GIFTBOX_OPEN, 1);

    leftPos = vec2{room->roomCenter[0]-175,room->roomCenter[1]};
    middlePos = vec2{room->roomCenter[0],room->roomCenter[1]};
    rightPos = vec2{room->roomCenter[0]+175,room->roomCenter[1]};  

    registry.books.emplace(openedBook_left);
    registry.books.emplace(openedBook_middle);
    registry.books.emplace(openedBook_right);

    fallingPoop = room->spawnEntity(vec2{room->roomCenter[0],room->roomCenter[1]-300}, vec2(40, 40), Textures::SOAP, 1);
    if(!registry.poops.has(fallingPoop)){
        registry.poops.emplace(fallingPoop);
    }
    if(!registry.motions.has(fallingPoop)){
        registry.motions.emplace(fallingPoop);
    }
    auto& motion = registry.motions.get(fallingPoop);
    motion.velocity.y += 150.f;

    fallingTime = false;
    shuffleTime = true;
}

void BathroomGameSystem::onhitBook() {    
    for(const Entity& poop : registry.poops.entities){
            registry.renderRequests.remove(poop);
            registry.poops.remove(poop);
    }
  
    for (Entity entity : registry.books.entities) {
        registry.remove_all_components_of(entity);
    }
    

    book_left = room->spawnEntity(leftPos, vec2(120, 120), Textures::GIFTBOX_CLOSE, 1);
    book_middle = room->spawnEntity(middlePos, vec2(120, 120), Textures::GIFTBOX_CLOSE, 1);
    book_right = room->spawnEntity(rightPos, vec2(120, 120), Textures::GIFTBOX_CLOSE, 1);

    registry.books.emplace(book_left);
    registry.books.emplace(book_middle);
    registry.books.emplace(book_right);

    leftPos = leftPos + room->roomMin;
    middlePos = middlePos + room->roomMin;
    rightPos = rightPos + room->roomMin;
    startShuffle();
}


float BathroomGameSystem::randomDuration(float minDuration, float maxDuration) {
    return minDuration + (std::rand() / (float)RAND_MAX) * (maxDuration - minDuration);
}

void BathroomGameSystem::displayEndMsg(bool isWrong) {
    std::cout << "really display end msg\n";
    displayMessageTimer = 4.0f;

    if(!isWrong){
        //displayText = room->spawnEntity(bathroom->roomCenter + vec2(0.0, bathroom->roomSize.y), vec2(140, 90) * 5.0f, Textures::BATHROOM_GAME_WIN_MSG, -5);
        chatboxSystem->createBox("You win! You've received soap!\nYou've also received fertilizer\nin the backyard!", vec2(-275.f, -45.f));
        // std::cout << "You win!" << std::endl;
    } else {
        //displayText = room->spawnEntity(bathroom->roomCenter + vec2(0.0, bathroom->roomSize.y), vec2(140, 90) * 5.0f, Textures::BATHROOM_GAME_LOSE_MSG, -5);
        chatboxSystem->createBox("You lose!\nBut at least you received fertilizer\nin the backyard!", vec2(-275.f, -45.f));
        // std::cout << "You lose!" << std::endl;
    }

}
void BathroomGameSystem::displayUI(bool isDisplay) {
    if (!isDisplay) {
        for (Entity entity : registry.userInterfaces.entities) {
            RenderRequest& renderReq = registry.renderRequests.get(entity);
            renderReq.z_index += tempUIZIncrement;
        }
    }
    else {
        for (Entity entity : registry.userInterfaces.entities) {
            RenderRequest& renderReq = registry.renderRequests.get(entity);
            renderReq.z_index -= tempUIZIncrement;
        }
    }
}

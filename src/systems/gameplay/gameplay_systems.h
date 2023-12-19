//
// Created by Tyler on 2023-10-30.
//

#pragma once

#include "mail_system.h"
#include "parallax_system.h"
#include "pickup_system.h"
#include "farm_system.h"
#include "buddy_status_system.h"
#include "save_system.h"
#include "bathroom_game_system.h"
#include "chatbox_system.h"
#include "intro_system.h"
#include "end_system.h"

class GameplaySystems {

public:

    static void step(float elapsed_ms);

    static void initializeGameplaySystems(WorldSystem* worldSystem, RenderSystem* renderSystem);

    static void destroyGameplaySystems();
};

extern MailSystem* mailSystem;
extern ParallaxSystem* parallaxSystem;
extern DreamGameSystem* dreamGameSystem;
extern KitchenGameSystem* kitchenGameSystem;
extern BackyardGameSystem* backyardGameSystem;
extern PickupSystem* pickupSystem;
extern FarmSystem* farmSystem;
extern BuddyStatusSystem* buddyStatusSystem;
extern WorldSystem* worldSystem;
extern BathroomGameSystem* bathroomGameSystem;
extern ChatboxSystem* chatboxSystem;
extern IntroSystem* introSystem;
extern EndSystem* endSystem;

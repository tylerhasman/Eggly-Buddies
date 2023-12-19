#pragma once


#include "systems/core/world_system.hpp"
#include "nlohmann/json.hpp"
#include "systems/gameplay/buddy_status_system.h"

class SaveSystem {

public:

	static void saveBuddyStatus();
	static void load();

    static bool isNewSave;

private:
	
};
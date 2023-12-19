#include "engine/tiny_ecs_registry.hpp"
#include "save_system.h"
#include "gameplay_systems.h"
#include "systems/gameplay/inventory_system.h"
// stlib
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
using json = nlohmann::json;
using namespace std;


#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#define GET_CWD _getcwd
// Check if the platform is Mac or Linux
#elif defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#define GET_CWD getcwd
#endif

bool SaveSystem::isNewSave = false;

void SaveSystem::saveBuddyStatus() {

	char cwd[1024];
	if (GET_CWD(cwd, sizeof(cwd)) != nullptr) {
		cout << "Current working dir: " << cwd << endl;
	}
	else {
		cerr << "getcwd() error" << endl;
	}

	string savePath = string(PROJECT_SOURCE_DIR) + "/data/game.json";

	json Buddy;

	if (!buddyStatusSystem) {
		cerr << "Error: 'buddy' is a nullptr" << endl;
		return;
	}

	// Log the values before saving
	// cout << "Tiredness: " << buddyStatusSystem->tiredness << endl;
	// cout << "Dirtiness: " << buddyStatusSystem->dirtiness << endl;
	// cout << "Hunger: " << buddyStatusSystem->dirtiness << endl;

	// Buddy["tiredness"] = buddyStatusSystem->tiredness;
	// Buddy["dirtiness"] = buddyStatusSystem->dirtiness;
	// Buddy["hunger"] = buddyStatusSystem->hunger;

	cout << "rest: " << buddyStatusSystem->rest << endl;
	cout << "hygiene: " << buddyStatusSystem->hygiene << endl;
	cout << "satiation: " << buddyStatusSystem->satiation << endl;

	Buddy["rest"] = buddyStatusSystem->rest;
	Buddy["hygiene"] = buddyStatusSystem->hygiene;
	Buddy["satiation"] = buddyStatusSystem->satiation;
	Buddy["entertainment"] = buddyStatusSystem->entertainment;
    Buddy["personality"] = (int) buddyStatusSystem->personality;
    Buddy["stage"] = (int) buddyStatusSystem->growthStage;
    Buddy["age"] = buddyStatusSystem->age;
    Buddy["level"] = backyardGameSystem->backyardLevel;
	Buddy["Fertilizer"] = Inventory::getItemAmount(InventoryItemType::Fertilizer);
	Buddy["Food"] = Inventory::getItemAmount(InventoryItemType::Food);

	ofstream file(savePath);
	if (file) {
		file << fixed << setprecision(2) << Buddy.dump(4);
	}
	else {
		cerr << "open file error" << endl;
	}


}

void SaveSystem::load() {

	string loadPath = string(PROJECT_SOURCE_DIR) + "/data/game.json";


	ifstream ifs(loadPath);

	try {
        isNewSave = false;
		if (!ifs) {
			// File does not exist or cannot be opened, so create it with default values
			std::ofstream ofs(loadPath);
			ofs << "{\n"
                   "\"entertainment\":100,\n"
                   "\"hygiene\":100,\n"
                   "\"rest\":100,\n"
                   "\"satiation\":100,\n"
                   "\"personality\":0,\n"
                   "\"stage\":0,\n"
                   "\"age\":0,\n"
				   "\"Fertilizer\":4,\n"
				   "\"Food\":0,\n"
				   "\"level\":1\n"
                   "}";
			ofs.close();
			std::cout << "New file created with default values: " << loadPath << std::endl;
			ifs.open(loadPath); // Re-open the file for reading
            isNewSave = true;
		}

		// Parse the JSON
		json j = json::parse(ifs);
		//https://json.nlohmann.me/api/basic_json/is_discarded/
		if (j.is_discarded()) {
			throw std::runtime_error("JSON parsing error");
		}

		// Close the file stream
		ifs.close();

		// Access values
		float hygiene = j["hygiene"].get<float>();
		float rest = j["rest"].get<float>();
		float satiation = j["satiation"].get<float>();
		float entertainment = j["entertainment"].get<float>();
		int fertilizer = j["Fertilizer"].get<int>();
		int food = j["Food"].get<int>();

		// for each value, if the saved value > 100, set to 99
		// for each value, if the saved value < 0, set to 0.1
		if (hygiene > buddyStatusSystem->MAX_STAT) {
			hygiene = 99.f;
		} else if (hygiene < buddyStatusSystem->MIN_STAT) {
			hygiene = 0.1f;
		}

		if (rest > buddyStatusSystem->MAX_STAT) {
			rest = 99.f;
		} else if (rest < buddyStatusSystem->MIN_STAT) {
			rest = 0.1f;
		}

		if (satiation > buddyStatusSystem->MAX_STAT) {
			satiation = 99.f;
		} else if (satiation < buddyStatusSystem->MIN_STAT) {
			satiation = 0.1f;
		}

		if (entertainment > buddyStatusSystem->MAX_STAT) {
			entertainment = 99.f;
		} else if (entertainment < buddyStatusSystem->MIN_STAT) {
			entertainment = 0.1f;
		}
        int personality = j["personality"].get<int>();
        int stage = j["stage"].get<int>();
        float age = j["age"].get<float>();

		buddyStatusSystem->hygiene = hygiene;
		buddyStatusSystem->rest = rest;
		buddyStatusSystem->satiation = satiation;
		buddyStatusSystem->entertainment = entertainment;
		buddyStatusSystem->personality = (Personality) personality;
		buddyStatusSystem->growthStage = (BuddyGrowthStage) stage;
		buddyStatusSystem->age = age;
		int level = j["level"].get<int>();
		backyardGameSystem->backyardLevel = level;
		Inventory::setItemAmount(InventoryItemType::Fertilizer, fertilizer);
		Inventory::setItemAmount(InventoryItemType::Food, food);


	}
	catch (const std::exception& e) {
		std::cerr << "Exception occurred: " << e.what() << std::endl;
	}

}


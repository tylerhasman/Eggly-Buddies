#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<vec3> colors;
	ComponentContainer<Scrolling> scrolling;
	ComponentContainer<Player> players;
	ComponentContainer<MailEvent> mailEvents;
	ComponentContainer<SleepState> sleepStates;
	ComponentContainer<DreamEnemy> dreamEnemies;
	ComponentContainer<FallingFood> fallingFoods;
	ComponentContainer<KitchenEvent> kitchenEvents;
	ComponentContainer<KitchenAppliance> kitchenAppliances;
	ComponentContainer<Food> foods;
	ComponentContainer<Clickable> clickables;
	ComponentContainer<Pickupable> pickupables;
	ComponentContainer<Boundary> boundaries;
	ComponentContainer<EgglyStatus> egglystatus;
	/*
	ComponentContainer<Dirtiness> dirtiness;
	ComponentContainer<Tiredness> tiredness;
	ComponentContainer<Hunger> hunger;
	*/
	ComponentContainer<Satiation> satiation;
	ComponentContainer<Hygiene> hygiene;
	ComponentContainer<Rest> rest;
	ComponentContainer<Entertainment> entertainment;
    ComponentContainer<Camera> cameras;
    ComponentContainer<UserInterface> userInterfaces;
	ComponentContainer<Growable> growables;
	ComponentContainer<Fertilizer> fertilizers;
    ComponentContainer<SpriteSheet> spriteSheets;
	ComponentContainer<CleanTool> cleanTools;
	ComponentContainer<UserHelp> userHelpPages;
	ComponentContainer<StatusBar> statusBars;
    ComponentContainer<CollisionHandler> collisionHandlers;
    ComponentContainer<PickupReceiver> pickupReceivers;
    ComponentContainer<AiController> aiControllers;
    ComponentContainer<Text> texts;
	ComponentContainer<Poop> poops;
	ComponentContainer<Book> books;
	ComponentContainer<Toilet> toilets;
    ComponentContainer<Projectile> projectiles;
    ComponentContainer<BackyardEnemy> backyardEnemies;
    ComponentContainer<HitTimer> hitTimers;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&colors);
		registry_list.push_back(&players);
		registry_list.push_back(&mailEvents);
		registry_list.push_back(&sleepStates);
		registry_list.push_back(&dreamEnemies);
		registry_list.push_back(&fallingFoods);
		registry_list.push_back(&kitchenEvents);
		registry_list.push_back(&kitchenAppliances);
		registry_list.push_back(&foods);
		registry_list.push_back(&scrolling);
		registry_list.push_back(&clickables);
		registry_list.push_back(&pickupables);
		registry_list.push_back(&boundaries);
		registry_list.push_back(&egglystatus);
		// registry_list.push_back(&dirtiness);
		// registry_list.push_back(&tiredness);
		// registry_list.push_back(&hunger);
		registry_list.push_back(&satiation); //new
		registry_list.push_back(&hygiene); //new
		registry_list.push_back(&rest); //new
		registry_list.push_back(&entertainment);   //new
        registry_list.push_back(&cameras);
        registry_list.push_back(&userInterfaces);
		registry_list.push_back(&growables);
		registry_list.push_back(&fertilizers);
        registry_list.push_back(&spriteSheets);
		registry_list.push_back(&cleanTools);
		registry_list.push_back(&userHelpPages);
		registry_list.push_back(&statusBars);
        registry_list.push_back(&collisionHandlers);
        registry_list.push_back(&pickupReceivers);
        registry_list.push_back(&aiControllers);
        registry_list.push_back(&texts);
		registry_list.push_back(&poops);
		registry_list.push_back(&toilets);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&backyardEnemies);
		registry_list.push_back(&hitTimers);

	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;

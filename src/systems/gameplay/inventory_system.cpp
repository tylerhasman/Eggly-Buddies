//
// Created by Tyler on 2023-11-19.
//

#include "inventory_system.h"

#include <unordered_map>
#include <iostream>
#include <sstream>

static std::unordered_map<InventoryItemType, int> inventory;

int Inventory::getItemAmount(InventoryItemType itemType) {
    if(inventory.find(itemType) == inventory.end()){
        return 0;
    }
    return inventory[itemType];
}

int Inventory::adjustItemAmount(InventoryItemType itemType, int amount) {
    int currentAmount = getItemAmount(itemType);

    inventory[itemType] = currentAmount + amount;

#ifdef ENABLE_INVENTORY_DEBUG
    std::cout << "Player now has " << inventory[itemType] << " " << getItemName(itemType) << std::endl;
#endif

    return inventory[itemType];
}


int Inventory::setItemAmount(InventoryItemType itemType, int amount) {
    inventory[itemType] = amount;

#ifdef ENABLE_INVENTORY_DEBUG
    std::cout << "Player now has " << inventory[itemType] << " " << getItemName(itemType) << std::endl;
#endif

    return inventory[itemType];
}

std::string Inventory::getItemName(InventoryItemType itemType) {
    switch(itemType){
        case Food:
            return "Food";
        case Seeds:
            return "Seeds";
        case Fertilizer:
            return "Fertilizer";
    }
    return "Unknown Item " + std::to_string(itemType);
}

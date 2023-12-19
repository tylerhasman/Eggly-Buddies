//
// Created by Tyler on 2023-11-19.
//

#pragma once

#include <string>

#define ENABLE_INVENTORY_DEBUG

enum InventoryItemType {
    Food,
    Seeds,
    Fertilizer
};

class Inventory {

public:
    // Get the current amount of an item in the inventory
    static int getItemAmount(InventoryItemType itemType);

    // Adjust the amount of an item in the inventory, returns the adjusted amount
    static int adjustItemAmount(InventoryItemType itemType, int amount);

    static int setItemAmount(InventoryItemType itemType, int amount);

    static std::string getItemName(InventoryItemType itemType);

};

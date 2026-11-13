#pragma once
#include <iostream>

struct Item {
    std::string name;
    double itemID;
    float itemWeight;
    bool stackable;
    int maxStackSize;
    int stackSize;
    std::string itemDescription;
    float stackWeight() {
        return stackSize * itemWeight;

    }
    int addToStack(int amount) {
        if (!stackable) {
            int newSize(stackSize + amount);
                if (newSize > maxStackSize) {
                    return maxStackSize;
                } else {
                    return newSize;
                }
        } 
    }

    int removeFromStack(int amount) {
        int newSize(stackSize - amount);
        if (newSize < 0) {
            return 0;
        } else {
            return newSize;
        }
    }
};
    
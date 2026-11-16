#pragma once
#include <iostream>
#include <algorithm>

struct Item {
    std::string name;
    std::string itemID;
    float itemWeight;
    int maxStackSize;
    int stackSize;
    std::string itemDescription;
    float stackWeight() {
        return stackSize * itemWeight;
    }

    bool isStackable() const {
        return maxStackSize > 1;
    }

    int addToStack(int amount) {
        if (!isStackable() || amount <= 0 || stackSize >= maxStackSize) {
            return stackSize;
        }

        const int room = maxStackSize - stackSize;
        stackSize += std::min(amount, room);
        return stackSize;
    }

    int removeFromStack(int amount) {
        int newSize(stackSize - amount);
        if (newSize < 0) {
            return 0;
        } else {
            return stackSize = newSize;
        }
    }
};
    
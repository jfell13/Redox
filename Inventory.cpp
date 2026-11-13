#include <Inventory.h>
#include <Item.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <map>

Inventory::Inventory(std::size_t initialCapacity)
    : slots_(initialCapacity) {}

bool Inventory::addItem(const Item& item) {
    if (item.stackSize <= 0 ||
        (item.stackable && item.stackSize > item.maxStackSize) ||
        (!item.stackable && item.stackSize != 1)) {
        return false;
    }

    for (auto& slot : slots_) {
        if (slot && item.stackable && slot->stackable &&
            slot->itemID == item.itemID &&
            item.stackSize <= slot->maxStackSize - slot->stackSize) {
            slot->stackSize += item.stackSize;
            return true;
        }
    }

    const auto emptySlot = std::find_if(
        slots_.begin(), slots_.end(),
        [](const auto& slot) { return !slot.has_value(); }
    );

    if (emptySlot == slots_.end()) {
        return false;
    }

    *emptySlot = item;
    return true;
}

// bool Inventory::removeItem(Item item) { 
// }

// int Inventory::countItem(Item item) {

// }
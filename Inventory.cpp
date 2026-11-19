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
        (item.isStackable() && item.stackSize > item.maxStackSize) ||
        (!item.isStackable() && item.stackSize != 1)) {
        return false;
    }

    for (auto& slot : slots_) {
        if (slot && item.isStackable() && slot->isStackable() &&
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

bool Inventory::removeItem(std::size_t index) {

    if (index >= slots_.size()) {
        return false;
    }

    auto& slot = slots_[index];
    
    if (!slot.has_value()) {
        return false;
    } 

    slot.reset();
    return true;
    // if (slot->stackSize >= 2) {
    //     slot->stackSize -= 1;
    //     return true;
    // } else {
    //     slot.reset();
    //     return true;
    // }
}

std::size_t Inventory::slotCount() const {
    return slots_.size();
}

const Item* Inventory::itemAt(std::size_t index) const {
    if (index >= slots_.size()) {
        return nullptr;
    }

    const auto& slot = slots_[index];
    return slot ? &*slot : nullptr;
}

const Item* Inventory::itemLookup(std::string_view name) const {
        for (const auto& slot : slots_) {
            if (slot && slot->name == name) {
                return &*slot;
            }
        }
        return nullptr;
    }
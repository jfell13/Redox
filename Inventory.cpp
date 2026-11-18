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

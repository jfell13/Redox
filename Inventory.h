#ifndef INVENTORY_H
#define INVENTORY_H
#include <Item.h>
#include <iostream>
#include <array>
#include <vector>
#include <optional> 
#include <cstddef>
#include <unordered_map>
#include <map>

// template <std::size_t Capacity>

class Inventory {
public:
    // Inventory() : 
    explicit Inventory(std::size_t initialCapacity);
    bool addItem(const Item& item);
    std::size_t slotCount() const;
    const Item* itemAt(std::size_t index) const;
    const std::vector<std::optional<Item>>& currentInventory() const {return slots_; }
    const Item* itemLookup(std::string_view name) const {
        for (const auto& slot : slots_) {
            if (slot && slot->name == name) {
                return &*slot;
            }
        }
        return nullptr;
    }
private:
    std::vector<std::optional<Item>> slots_{};
};

#endif
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
    bool Inventory::addItem(const Item& item);
    // bool removeItem(Item item);
    // int countItem(Item item) const;

private:
    std::vector<std::optional<Item>> slots_{};
};

#endif
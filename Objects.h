#ifndef OBJECTS_H
#define OBJECTS_H
#include <Inventory.h>
#include <Item.h>
#include <Model.h>
#include <optional>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>


class Container{
    public:
        explicit Container(
            const glm::vec3& spawnPosition,
            const std::size_t initialCapacity,
            std::string fileName,
            std::vector<Item> startingItems = {}
        );
        std::size_t slotCount() const { return inventory_.slotCount(); }
        const Inventory& currentInventory() const {return inventory_; }
        bool addItem(const Item& item) { return inventory_.addItem(item); }
        bool removeItem(std::size_t index) { return inventory_.removeItem(index); }
    private:
        glm::vec3 position_;
        Inventory inventory_;
        Model model_;
        // ClickableModel click_;

};

class Wall{};

#endif
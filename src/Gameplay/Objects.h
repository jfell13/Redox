#ifndef OBJECTS_H
#define OBJECTS_H
#include <Inventory.h>
#include <InventoryWindow.h>
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
        const Item* itemAt(std::size_t index) const { return inventory_.itemAt(index); }

        std::optional<std::size_t> showInventory(const std::string& title) {
            return inventoryWindow_.window(inventory_, title, &invOpen);
        }

        const glm::vec3 modelOrigin() const; 
        glm::mat4 modelTransform() const;
        const Model& model() const { return model_; }
        void cleanup() { 
            inventoryWindow_.cleanup();
            model_.cleanup(); 
        }

        bool isClicked(
            const glm::vec3& rayOrigin,
            const glm::vec3& rayDir,
            float& hitDistance
        );
        const glm::vec3& position() const { return position_; }
        bool invOpen = false;
    private:
        glm::vec3 position_;
        Inventory inventory_;
        Model model_;
        std::optional<ClickableModel> clicker_;
        InventoryWindow inventoryWindow_;
        const float modelScale = 1.0f;//0.05f;

};

class Wall{};

#endif
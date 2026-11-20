#include <Objects.h>
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
#include <stdexcept>

Container::Container(
            const glm::vec3& spawnPosition,
            const std::size_t initialCapacity,
            std::string fileName,
            std::vector<Item> startingItems
)

    : position_(spawnPosition),
    inventory_(initialCapacity),
    model_() {
    for (const Item& item : startingItems) {
        if (!inventory_.addItem(item)) {
            throw std::runtime_error(
                "Could not add starting item: " + item.name
            );
        } 
    }
    if (!model_.load(fileName)) {
        std::cerr << model_.error() << '\n';
        model_.cleanup();
        throw std::runtime_error(
            "Could not load model: " + fileName
        );
    }
}
    
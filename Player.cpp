#include <Player.h>
#include <Controls.h>
#include <Inventory.h>
#include <Item.h>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <stdexcept>
#include <unordered_map>
#include <map>

Player::Player(
    const glm::vec3& spawnPosition,
    const std::size_t initialCapacity,
    std::vector<Item> startingItems
    // const std::unordered_map<std::string, int>& initialAttributes,
    // const std::unordered_map<std::string, int>& initialSkills,
    // const std::unordered_map<std::string, int>& initialMerits,
    // const std::unordered_map<std::string, int>& iniitalFlaws
) 
    : controls_(spawnPosition),
    inventory_(initialCapacity) {
    for (const Item& item : startingItems) {
        if (!inventory_.addItem(item)) {
            throw std::runtime_error(
                "Could not add starting item: " + item.name
            );
        } 
    }
}

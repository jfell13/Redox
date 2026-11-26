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
    clicker_.emplace(&model_);
}

const glm::vec3 Container::modelOrigin() const {
    const glm::vec3 modelCenter = (model_.minimum() + model_.maximum()) * 0.5f;
    const glm::vec3 modelOrigin(modelCenter.x, modelCenter.y, model_.minimum().z);
    return modelOrigin;
}

glm::mat4 Container::modelTransform() const {
    glm::mat4 modelTransform = glm::translate(
            glm::mat4(1.0f), 
            position_
        );
        modelTransform = glm::scale(
            modelTransform, 
            glm::vec3(modelScale)
        );
        
        return modelTransform = glm::translate(modelTransform, -modelOrigin());
}

bool Container::isClicked(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    float& hitDistance
) {
    if (!clicker_) {
        return false;
    }

    clicker_->transform = modelTransform();
    return clicker_->IsClicked(rayOrigin, rayDir, hitDistance);
}

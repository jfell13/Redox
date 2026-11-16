#ifndef PLAYER_H
#define PLAYER_H
#include <Controls.h>
#include <Inventory.h>
#include <Item.h>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>


class Player {
    public:
        explicit Player(
            const glm::vec3& spawnPosition,
            const std::size_t initialCapacity,
            std::vector<Item> startingItems = {}
            // const std::unordered_map<std::string, int>& initialAttributes,
            // const std::unordered_map<std::string, int>& initialSkills,
            // const std::unordered_map<std::string, int>& initialMerits,
            // const std::unordered_map<std::string, int>& iniitalFlaws
        );
        const glm::vec3& position() const { return controls_.position(); }
        void updateControls(GLFWwindow* window, float deltaTime) {
            controls_.updateControls(window, deltaTime);
        }
        glm::mat4 viewMatrix() const { return controls_.viewMatrix();}
        float yaw() const { return controls_.yaw(); }
        std::size_t slotCount() const { return inventory_.slotCount(); }
        const Inventory& currentInventory() const {return inventory_; }
    private:
        glm::vec3 position_;
        PlayerControls controls_;
        Inventory inventory_;
        // std::unordered_map<std::string, int> attributes_;
        // std::unordered_map<std::string, int> skills_;
        // std::unordered_map<std::string, int> merits_;
        // std::unordered_map<std::string, int> flaws_;
};

#endif
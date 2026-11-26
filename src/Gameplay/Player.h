#ifndef PLAYER_H
#define PLAYER_H
#include <Controls.h>
#include <Inventory.h>
#include <Item.h>
#include <Model.h>
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
            std::string fileName,
            std::vector<Item> startingItems = {}
            // const std::unordered_map<std::string, int>& initialAttributes,
            // const std::unordered_map<std::string, int>& initialSkills,
            // const std::unordered_map<std::string, int>& initialMerits,
            // const std::unordered_map<std::string, int>& iniitalFlaws
        );
        const glm::vec3& position() const { return controls_.position(); }
        void setPosition(const glm::vec3& position) {
            controls_.setPosition(position);
        }
        const glm::vec3 modelOrigin() const; 
        glm::mat4 modelTransform() const;
        const Model& model() const { return model_; }
        void cleanup() { return model_.cleanup(); }

        void updateControls(GLFWwindow* window, float deltaTime) {
            controls_.updateControls(window, deltaTime);
        }
        glm::mat4 viewMatrix() const { return controls_.viewMatrix();}
        float yaw() const { return controls_.yaw(); }

        std::size_t slotCount() const { return inventory_.slotCount(); }
        const Inventory& currentInventory() const {return inventory_; }
        bool addItem(const Item& item) { return inventory_.addItem(item); }
        bool removeItem(std::size_t index) { return inventory_.removeItem(index); }
        const Item* itemAt(std::size_t index) const { return inventory_.itemAt(index); }

        bool invOpen = false;
        // bool leftClick(GLFWwindow* window) {
        //     return controls_.leftClick(window);
        // }
        void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        glm::vec3 GetRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight, 
                          const glm::mat4& view, const glm::mat4& projection);

    private:
        // glm::vec3 position_;
        PlayerControls controls_;
        Inventory inventory_;
        Model model_;

        const float modelScale = 1.0f;
        // std::unordered_map<std::string, int> attributes_;
        // std::unordered_map<std::string, int> skills_;
        // std::unordered_map<std::string, int> merits_;
        // std::unordered_map<std::string, int> flaws_;
};

#endif
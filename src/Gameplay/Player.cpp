#include <Player.h>
#include <Controls.h>
#include <Inventory.h>
#include <Item.h>
#include <Model.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <array>
#include <stdexcept>
#include <unordered_map>
#include <map>

Player::Player(
    const glm::vec3& spawnPosition,
    const std::size_t initialCapacity,
    std::string fileName,
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
    if (!model_.load(fileName)) {
        std::cerr << model_.error() << '\n';
        model_.cleanup();
        throw std::runtime_error(
            "Could not load model: " + fileName
        );
    }
}

const glm::vec3 Player::modelOrigin() const {
    const glm::vec3 modelCenter = (model_.minimum() + model_.maximum()) * 0.5f;
    const glm::vec3 modelOrigin(modelCenter.x, modelCenter.y, model_.minimum().z);
    return modelOrigin;
}

glm::mat4 Player::modelTransform() const {
    const glm::vec3& playerPosition = position();
    glm::mat4 modelTransform = glm::translate(
            glm::mat4(1.0f), 
            glm::vec3(
                playerPosition.x, 
                playerPosition.y, 
                playerPosition.z
            )
        );
        modelTransform = glm::rotate(
            modelTransform,
            yaw(),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
        modelTransform = glm::scale(
            modelTransform, 
            glm::vec3(modelScale)
        );
        
        return modelTransform = glm::translate(modelTransform, -modelOrigin());
}

void Player::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
        std::cout << "Mouse Position: (" << xpos << ", " << ypos << ")\n";
}

void Player::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos); // Get position at the moment of click
        std::cout << "Left click at: (" << xpos << ", " << ypos << ")\n";
    }
}

glm::vec3 Player::GetRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight, 
                          const glm::mat4& view, const glm::mat4& projection) {
    // 1. Convert screen coordinates to Normalized Device Coordinates (-1 to 1)
    float x = (2.0f * static_cast<float>(mouseX)) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenHeight; // Invert Y for OpenGL

    // 2. Homogeneous Clip Coordinates
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    // 3. Eye (Camera) Coordinates
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // 4. World Coordinates
    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    return glm::normalize(rayWorld);
}
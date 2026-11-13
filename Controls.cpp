#include "Controls.h"
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include <imgui_internal.h>
// #include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

namespace {
constexpr float modelForwardAngle = glm::radians(-90.0f);
constexpr float modelTurnSpeed = glm::radians(120.0f);
constexpr float cameraSpeed = 1.0f;

bool pressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}
}

PlayerControls::PlayerControls(const glm::vec3& initialPosition)
    : position_(initialPosition),
      cameraAzimuth_(modelForwardAngle + glm::radians(180.0f)),
      cameraElevation_(glm::radians(25.0f)) {}

void PlayerControls::updateControls(GLFWwindow* window, float deltaTime) {
    updateCamera(window, deltaTime);
    updateMovement(window, deltaTime);
}

void PlayerControls::updateCamera(GLFWwindow* window, float deltaTime) {
    // L/R turn both camera and model.
    const float orbitInput =
        static_cast<float>(pressed(window, GLFW_KEY_LEFT)) -
        static_cast<float>(pressed(window, GLFW_KEY_RIGHT));
    cameraAzimuth_ += orbitInput * cameraSpeed * deltaTime;
    cameraAzimuth_ = std::remainder(cameraAzimuth_, glm::radians(360.0f));
    // Camera elevation.
    const float elevationInput =
        static_cast<float>(pressed(window, GLFW_KEY_UP)) -
        static_cast<float>(pressed(window, GLFW_KEY_DOWN));
    cameraElevation_ += elevationInput * cameraSpeed * deltaTime;
    cameraElevation_ = glm::clamp(
        cameraElevation_, 0.0f, glm::radians(89.0f)
    );
    // Camera zoom.
    const float zoomInput =
        static_cast<float>(pressed(window, GLFW_KEY_PERIOD)) -
        static_cast<float>(pressed(window, GLFW_KEY_COMMA));

    cameraRadius_ += zoomInput * 6.0f * deltaTime;
    cameraRadius_ = glm::clamp(cameraRadius_, 5.0f, 50.0f);
}

void PlayerControls::updateMovement(GLFWwindow* window, float deltaTime) {
    const float turnInput =
        static_cast<float>(pressed(window, GLFW_KEY_A)) -
        static_cast<float>(pressed(window, GLFW_KEY_D));
    yaw_ += turnInput * modelTurnSpeed * deltaTime;
    yaw_ = std::remainder(yaw_, glm::radians(360.0f));
    
    const bool movingForward = pressed(window, GLFW_KEY_W);
    const bool movingBackward = pressed(window, GLFW_KEY_S);

    if (movingForward != movingBackward) {
    // The orbit angle points from the player toward the camera.
    // Add half a turn to place it behind the model's forward direction.
        const float fullTurn = glm::radians(360.f);
        const float targetAzimuth =
            yaw_ + modelForwardAngle + glm::radians(180.0f);
        const float angleError =
            std::remainder(targetAzimuth - cameraAzimuth_, fullTurn);
        constexpr float followRate = 6.0f; // Per second.
        const float blend = 1.0f - std::exp(-followRate * deltaTime);
        cameraAzimuth_ = std::remainder(
            cameraAzimuth_ + angleError * blend, fullTurn
        );
    }

    const glm::vec3 forward(
        -std::cos(cameraAzimuth_),
        -std::sin(cameraAzimuth_),
        0.0f
    );
    const glm::vec3 right = glm::cross(
        forward, glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::vec3 movement(0.0f);
    if (movingForward)  movement += forward;
    if (movingBackward) movement -= forward;
    if (pressed(window, GLFW_KEY_Q)) movement -= right;
    if (pressed(window, GLFW_KEY_E)) movement += right;

    constexpr float movementSpeed = 3.0f;
    if (glm::length(movement) > 0.0f) {
        const glm::vec3 direction = glm::normalize(movement);
        position_ += direction * movementSpeed * deltaTime;
    }
    
}

glm::mat4 PlayerControls::viewMatrix() const {
    const glm::vec3 cameraPosition = calculateCameraPosition(
        cameraRadius_, cameraAzimuth_, cameraElevation_, position_);
    return glm::lookAt(cameraPosition, position_, glm::vec3(0.0f, 0.0f, 1.0f));
}

// void PlayerControls::viewInventory(bool& inventoryOpen) {
//     if (!io.WantTextInput && ImGui::IsKeyPressed(ImGuiKey_I, false)) {
//             inventoryOpen = !inventoryOpen;
//         }
//         if (inventoryOpen) {
//             ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

//             if (ImGui::Begin("Inventory", &inventoryOpen)) {
//                 if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0))) {
//                     for (int i = 0; i < 20; ++i) {
//                         ImGui::Text("Inventory slot %d", i + 1);
//                     }
//                 }
//                 ImGui::EndChild();
//             }
//             ImGui::End();
//         }
// }
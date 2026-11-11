#include "Controls.h"

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
    // A/D turn both camera and model.
    // Hold Left Alt to turn only the camera.
    const bool cameraOnly = pressed(window, GLFW_KEY_LEFT_ALT);
    const float turnInput =
        static_cast<float>(pressed(window, GLFW_KEY_A)) -
        static_cast<float>(pressed(window, GLFW_KEY_D));
    const float turnAngle = turnInput * modelTurnSpeed * deltaTime;
    cameraAzimuth_ += turnAngle;
    if (!cameraOnly)
        yaw_ += turnAngle;
    cameraAzimuth_ = std::remainder(cameraAzimuth_, glm::radians(360.0f));
    yaw_ = std::remainder(yaw_, glm::radians(360.0f));
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
    const glm::vec3 forward(
        -std::cos(cameraAzimuth_),
        -std::sin(cameraAzimuth_),
        0.0f
    );
    const glm::vec3 right = glm::cross(
        forward, glm::vec3(0.0f, 0.0f, 1.0f)
    );
    const bool movingForward = pressed(window, GLFW_KEY_W);
    const bool movingBackward = pressed(window, GLFW_KEY_S);

    glm::vec3 movement(0.0f);
    if (movingForward)  movement += forward;
    if (movingBackward) movement -= forward;
    if (pressed(window, GLFW_KEY_Q)) movement -= right;
    if (pressed(window, GLFW_KEY_E)) movement += right;

    constexpr float movementSpeed = 3.0f;

    if (glm::length(movement) > 0.0f) {
        const glm::vec3 direction = glm::normalize(movement);

        position_ += direction * movementSpeed * deltaTime;

        // Face travel direction when W or S is pressed alone.
        // Q/E alone keep their existing strafing behavior.
        if (movingForward != movingBackward) {
            yaw_ = std::atan2(direction.y, direction.x)
                    - modelForwardAngle;
        }
    }
}

glm::mat4 PlayerControls::viewMatrix() const {
    const glm::vec3 cameraPosition = calculateCameraPosition(
        cameraRadius_, cameraAzimuth_, cameraElevation_, position_);
    return glm::lookAt(cameraPosition, position_, glm::vec3(0.0f, 0.0f, 1.0f));
}

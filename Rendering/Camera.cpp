#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include <cmath>

glm::vec3 calculateCameraPosition(
    float cameraRadius,
    float cameraAzimuth,
    float cameraElevation,
    const glm::vec3& target
) {
    // float x = target.x 
    // + cameraRadius * cos(cameraElevation) * cos(cameraAzimuth);
    // float y = target.y + cameraRadius * sin(cameraElevation);
    // float z = target.z 
    // + cameraRadius * cos(cameraElevation) * sin(cameraAzimuth);
    // return glm::vec3(x, y, z);
    const float horizontalRadius = cameraRadius * std::cos(cameraElevation);
    const float x = target.x + horizontalRadius * std::cos(cameraAzimuth);
    const float y = target.y + horizontalRadius * std::sin(cameraAzimuth);
    const float z = target.z + cameraRadius * std::sin(cameraElevation);
    return glm::vec3(x, y, z);
}

// void cameraControls(
//     GLFWwindow* window, 
//     float& cameraAzimuth, 
//     float& cameraElevation, 
//     float& cameraRadius, 
//     float cameraSpeed,
//     float deltaTime
// ) {
//     if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//         cameraAzimuth -= cameraSpeed * deltaTime;
//     }
//     if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//         cameraAzimuth += cameraSpeed * deltaTime;
//     }
//     if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//         cameraElevation += cameraSpeed * deltaTime;
//     }
//     if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//         cameraElevation -= cameraSpeed * deltaTime;
//     }
//     if (glfwGetKey(window, GLFW_KEY_COMMA) 
//         //&& glfwGetKey(window, GLFW_KEY_UP)
//         == GLFW_PRESS
//     ) {
//         cameraRadius -= 6.0f * deltaTime;
//     }
//     if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
//         cameraRadius += 6.0f * deltaTime;
//     }
//     cameraRadius = glm::clamp(
//         cameraRadius,
//         5.0f,
//         50.0f
//     );
//     cameraElevation = glm::clamp(
//         cameraElevation,
//         glm::radians(0.0f),
//         glm::radians(89.0f)
//     );
// }
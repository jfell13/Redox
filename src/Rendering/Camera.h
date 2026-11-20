#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

glm::vec3 calculateCameraPosition(
    float radius,
    float azimuth,
    float elevation,
    const glm::vec3& target
);

// void cameraControls(
//     GLFWwindow* window,
//     float& azimuth,
//     float& elevation,
//     float& radius,
//     float speed,
//     float deltaTime
// );

#endif // CAMERA_H
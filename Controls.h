#ifndef CONTROLS_H
#define CONTROLS_H

#include <glm/glm.hpp>

struct GLFWwindow;

class PlayerControls {
public:
    explicit PlayerControls(const glm::vec3& initialPosition);

    void updateControls(GLFWwindow* window, float deltaTime);
    glm::mat4 viewMatrix() const;

    const glm::vec3& position() const { return position_; }
    float yaw() const { return yaw_; }

private:
    void updateCamera(GLFWwindow* window, float deltaTime);
    void updateMovement(GLFWwindow* window, float deltaTime);

    glm::vec3 position_;
    float yaw_ = 0.0f;
    float cameraAzimuth_;
    float cameraElevation_;
    float cameraRadius_ = 18.0f;
};

#endif // CONTROLS_H

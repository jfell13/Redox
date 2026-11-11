#ifndef CONTROLS_H
#define CONTROLS_H
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

class PlayerControls {

    // Constructor
    PlayerControls(
        GLFWwindow* window, 
        glm::vec3& movement, 
        glm::vec3 forward, 
        glm::vec3 right
    );
public:
    void updateControls(
        GLFWwindow* window, 
        glm::vec3& movement, 
        glm::vec3 forward, 
        glm::vec3 right
    );

    void openWindow(GLFWwindow* window);

};

#endif // CONTROLS_H
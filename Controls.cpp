#include <Controls.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

PlayerControls::PlayerControls(
    GLFWwindow* window, 
    glm::vec3& movement, 
    glm::vec3 forward, 
    glm::vec3 right
) {
    updateControls(window, movement, forward, right);
}

void PlayerControls::updateControls(
    GLFWwindow* window, 
    glm::vec3& movement, 
    glm::vec3 forward, 
    glm::vec3 right
) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement -= forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement += right;
}

void PlayerControls::openWindow(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        // open inventory window logic here
        InventoryWindow inventoryWindow;
        inventoryWindow.open();
}
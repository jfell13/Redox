#include "MenuBar.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

void MenuBar::menuBar(GLFWwindow* window) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Func1")) {
                // Things
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Other1")) {
            if (ImGui::MenuItem("Func2")) {
                // Things
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

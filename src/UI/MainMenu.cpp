#include <MainMenu.h>
#include <UIElements.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utility>

void MainMenu::configureMainMenu() {
    // ImGui::SetNextWindowPos(ImVec2(xStreamPos, yStreamPos), ImGuiCond_FirstUseEver);
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowSize(
        ImVec2(viewport->WorkSize.x * 0.4f,
            viewport->WorkSize.y * 0.6f),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowPos(
        viewport->GetCenter(),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );
    // ImGui::SetNextWindowSize(ImVec2(xStreamSize, yStreamSize), ImGuiCond_FirstUseEver);
    // ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
}

void MainMenu::draw() {
    configureMainMenu();
    if (ImGui::Begin("Main Menu")) {
        if (ImGui::Button("New Game")) {
            std::cout << "New Game Button was pressed!" << std::endl;
            Game_ = true;
        }
        if (ImGui::Button("Continue")) {
            std::cout << "Continue Button was pressed!" << std::endl;
            Game_ = true;
        }
        if (ImGui::Button("Load Game")) {
            std::cout << "Load Game Button was pressed!" << std::endl;
        }
        if (ImGui::Button("Settings")) {
            std::cout << "Settings Button was pressed!" << std::endl;
        }
        if (ImGui::Button("Exit")) {
            std::cout << "Exit Button was pressed!" << std::endl;
            Quit_ = true;
        }
    }
    ImGui::End();
}

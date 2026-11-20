#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utility>
#include <LoggerWindow.h>

LoggerWindow::LoggerWindow(
        float init_xStreamPos,
        float init_yStreamPos,
        float init_xStreamSize,
        float init_yStreamSize
)
    : xStreamPos(init_xStreamPos),
    yStreamPos(init_yStreamPos),
    xStreamSize(init_xStreamSize),
    yStreamSize(init_yStreamSize)
{}
    // Methods
void LoggerWindow::configureLogWindow() {
    ImGui::SetNextWindowPos(ImVec2(xStreamPos, yStreamPos), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(xStreamSize, yStreamSize), ImGuiCond_FirstUseEver);
}

void LoggerWindow::addText(const char* str, double charSize, int side, glm::vec3 textCol) {
	GUIelement element;
	element.type = GUI_TEXT;
	element.data = nullptr;
	element.charSize = charSize;
	element.string = str;
	element.min = -1.0;
	element.max = -1.0;
	element.side = side;
	element.textColor = textCol;
	element.backgroundColor = glm::vec3(0.0);
	elements.push_back(element);
}

void LoggerWindow::addMessage(std::string message) {
    entries.push_back({std::move(message)});
    while (entries.size() > maxEntries)
        entries.pop_front();
}

void LoggerWindow::draw() {
    configureLogWindow();
    if (ImGui::Begin("Output Log")) {
        for (const auto& entry : entries) {
            ImGui::TextUnformatted(entry.message.c_str());
            // ImGui::PushStyleColor(
            //     ImGuiCol_Text, 
            //     ImVec4(
            //         entry.textColor.r, 
            //         element.textColor.g, 
            //         element.textColor.b, 
            //         1.0f
            //     )
            // );
            // ImGui::TextUnformatted();
            // ImGui::PopStyleColor();
        }
    }
    ImGui::End();
}

// void LoggerWindow::draw() {
//     configureLogWindow();
//     if (ImGui::Begin("Output Log")) {
//         for (const auto& element : elements) {
//             if (element.type != GUI_TEXT)
//                 continue;
//             const float availableWidth = ImGui::GetContentRegionAvail().x;
//             const float textWidth = ImGui::CalcTextSize(element.string.c_str()).x;
//             const float extraWidth = availableWidth - textWidth;
//             if (extraWidth > 0.0f) {
//                 if (element.side == GUI_CENTER_SIDE)
//                     ImGui::SetCursorPosX(ImGui::GetCursorPosX() + extraWidth * 0.5f);
//                 else if (element.side == GUI_RIGHT_SIDE)
//                     ImGui::SetCursorPosX(ImGui::GetCursorPosX() + extraWidth);
//             }
//             ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(
//                 element.textColor.r, element.textColor.g, element.textColor.b, 1.0f));
//             ImGui::TextUnformatted(element.string.c_str());
//             ImGui::PopStyleColor();
//         }
//     }
//     ImGui::End();
// }

// if (ImGui::Button("Click Me!")) {
//     std::cout << "Button was pressed!" << std::endl;
//     addText("Button was pressed!", 0.05, GUI_CENTER_SIDE, glm::vec3(1.0));
// }
#include <filesystem>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "stb_image.h"
#include <unordered_map>
#include <utility>
#include <Player.h>
#include <PlayerWindow.h>
#include <UIElements.h>

std::optional<std::size_t> PlayerWindow::playerWindow(Player& player) {
    std::optional<std::size_t> clickedSlot;
    ImGui::SetNextWindowSize(ImVec2(500, 1200), ImGuiCond_FirstUseEver);
    const std::filesystem::path iconPath =
                            std::filesystem::path(ReDox_ICONS_PATH);
    const auto bodyPath = iconPath / "body-template.png";
    auto [entry, inserted] = iconTextures_.try_emplace(bodyPath.string(), 0);
    if (inserted) {
        entry->second = loadIconTexture(bodyPath);
    }
    const GLuint bodyTexture = entry->second;
    const std::string windowTitle = "Player Window";
    if (ImGui::Begin(windowTitle.c_str(), &player.invOpen)) {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
        const float topHeight = (available.y - spacing.y) * 0.5f;
        const ImVec2 slotSize(40.0f, 40.0f);
        if (ImGui::BeginChild("PlayerTop", ImVec2(0.0f, topHeight))) {
            // Player Equipment : TOP LEFT
            const float leftWidth =
                (ImGui::GetContentRegionAvail().x - spacing.x) * 0.5f;
            if (ImGui::BeginChild("Equipment", ImVec2(leftWidth, 0.0f))) {
                // float bodyWidth = leftWidth - padding.x * 2.0f;
                // float bodyHeight = 4 * bodyWidth / 3;
                ImGui::TextUnformatted("Equipment");
                const ImVec2 imageOrigin = ImGui::GetCursorPos();
                const ImVec2 imageSize(180.0f, 240.0f);
                ImGui::Image(static_cast<ImTextureID>(bodyTexture),imageSize);
                // Draw helmet slot here.
                const ImVec2 helmetAnchor(0.5f, 0.12f);
                ImGui::SetCursorPos(ImVec2(
                    imageOrigin.x + imageSize.x * helmetAnchor.x - slotSize.x * 0.5f,
                    imageOrigin.y + imageSize.y * helmetAnchor.y - slotSize.y * 0.5f
                ));  
                ImGui::Button("##HeadSlot",slotSize);
                // Draw body slot here.
                const ImVec2 torsoAnchor(0.5f, 0.4f);
                ImGui::SetCursorPos(ImVec2(
                    imageOrigin.x + imageSize.x * torsoAnchor.x - slotSize.x * 0.5f,
                    imageOrigin.y + imageSize.y * torsoAnchor.y - slotSize.y * 0.5f
                ));  
                ImGui::Button("##TorsoSlot",slotSize);
                // Draw weapon slot here.
                const ImVec2 weaponAnchor(0.75f, 0.75f);
                ImGui::SetCursorPos(ImVec2(
                    imageOrigin.x + imageSize.x * weaponAnchor.x - slotSize.x * 0.5f,
                    imageOrigin.y + imageSize.y * weaponAnchor.y - slotSize.y * 0.5f
                ));  
                ImGui::Button("##WeaponSlot",slotSize);
            }
            ImGui::EndChild(); // Ending Equipment
            ImGui::SameLine();
            // Player Stats : TOP RIGHT
            if (ImGui::BeginChild("Stats", ImVec2(0.0f, 0.0f))) {
                ImGui::TextUnformatted("Stats");
                // Draw player stats here.
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
        // Player INVENTORY : BOTTOM HALF
        if (ImGui::BeginChild("PlayerInventory", ImVec2(0.0f, 0.0f))) {
            ImGui::TextUnformatted("Inventory");
            // inventoryWindow_.drawContents(player.currentInventory());
            clickedSlot =
                inventoryWindow_.drawContents(player.currentInventory());
        }
        ImGui::EndChild();
    }
    ImGui::End();
    return clickedSlot;
}

void PlayerWindow::playerWindowCleanup() {
    inventoryWindow_.cleanup();
    for (auto& [name, id] : iconTextures_) {
        glDeleteTextures(1, &id);
    }
    iconTextures_.clear();
}

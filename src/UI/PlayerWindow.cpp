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

GLuint loadIconTexture(const std::filesystem::path& path) {
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixels = stbi_load(
        path.string().c_str(),
        &width, &height, &channels,
        STBI_rgb_alpha
    );

    if (!pixels) {
        std::cerr << "Could not load icon: " << path
                  << ": " << stbi_failure_reason() << '\n';
        return 0;
    }

    GLint previousTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels
    );

    glBindTexture(GL_TEXTURE_2D, previousTexture);
    stbi_image_free(pixels);

    return texture;
}

void PlayerWindow::playerWindow(Player& player) {
    const ImVec2 padding = ImGui::GetStyle().FramePadding;
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
            // ImGui::Separator();
            if (ImGui::BeginChild("Stats", ImVec2(0.0f, 0.0f))) {
                ImGui::TextUnformatted("Stats");
                // Draw player stats here.
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
        
        if (ImGui::BeginChild("PlayerInventory", ImVec2(0.0f, 0.0f))) {
            ImGui::TextUnformatted("Inventory");
            constexpr int columns = 4;
            if (ImGui::BeginTable(
                "Inventory Slots", columns,
                ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)
            ) {
                for (std::size_t i = 0; i < player.currentInventory().slotCount(); ++i) {
                    ImGui::TableNextColumn();
                    ImGui::PushID(static_cast<int>(i));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    
                    const Item* item = player.currentInventory().itemAt(i);
                    if (item) {
                        // const std::filesystem::path iconPath =
                        //     std::filesystem::path(ReDox_ICONS_PATH) / item->thumbnail;
                        std::filesystem::path itemPath = iconPath / item->thumbnail;
                        const std::string key = itemPath.string();
                        auto [entry, inserted] = iconTextures_.try_emplace(key, 0);
                        if (inserted) {
                            entry->second = loadIconTexture(itemPath);
                        }
                        if (entry->second != 0) {
                            // ImGui::PushID(static_cast<int>(i));
                            if (ImGui::ImageButton(
                                    "ItemIcon",
                                    static_cast<ImTextureID>(entry->second),
                                    slotSize)) {
                                    // ImVec2(48.0f, 48.0f))) {
                                // Handle clicking this inventory slot.
                            }
                        } else {
                            ImGui::Button(
                            item->name.c_str(),
                            ImVec2(slotSize.x + padding.x * 2.0f,
                                slotSize.y + padding.y * 2.0f)
                            );
                        }
                    } else {
                        ImGui::Button(
                            "Empty",
                            ImVec2(slotSize.x + padding.x * 2.0f,
                                slotSize.y + padding.y * 2.0f)
                        );
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void PlayerWindow::playerWindowCleanup() {
    for (auto& [name, id] : iconTextures_) {
        glDeleteTextures(1, &id);
    }
    iconTextures_.clear();
}

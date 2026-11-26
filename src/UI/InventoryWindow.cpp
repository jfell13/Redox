#include "InventoryWindow.h"
#include "Inventory.h"
#include "UIElements.h"
#include <filesystem>
#include "imgui.h"

std::optional<std::size_t> InventoryWindow::window(
    const Inventory& inventory, const std::string& inventoryName, bool* open) {
    if (open && !*open) {
        return std::nullopt;
    }

    std::optional<std::size_t> clickedSlot;
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(inventoryName.c_str(), open)) {
        clickedSlot = drawContents(inventory);
    }
    ImGui::End();
    return clickedSlot;
}

std::optional<std::size_t> InventoryWindow::drawContents(const Inventory& inventory) {
    std::optional<std::size_t> clickedSlot;
    const ImVec2 slotSize(40.0f, 40.0f);
    const ImVec2 padding = ImGui::GetStyle().FramePadding;
    const ImVec2 buttonSize(
        slotSize.x + padding.x * 2.0f, slotSize.y + padding.y * 2.0f);

    if (ImGui::BeginTable("Inventory Slots", 4,
                         ImGuiTableFlags_Borders |
                         ImGuiTableFlags_SizingStretchSame)) {
        for (std::size_t i = 0; i < inventory.slotCount(); ++i) {
            ImGui::TableNextColumn();
            // Use a string ID to preserve the complete slot index.
            const std::string slotId = std::to_string(i);
            ImGui::PushID(slotId.c_str());
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

            const Item* item = inventory.itemAt(i);
            GLuint texture = 0;
            if (item && !item->thumbnail.empty()) {
                const auto path =
                    std::filesystem::path(ReDox_ICONS_PATH) / item->thumbnail;
                auto [entry, inserted] = iconTextures_.try_emplace(path.string(), 0);
                if (inserted) {
                    entry->second = loadIconTexture(path);
                }
                texture = entry->second;
            }

            bool clicked = false;
            if (texture != 0) {
                clicked = ImGui::ImageButton(
                    "ItemIcon", static_cast<ImTextureID>(texture), slotSize);
            } else {
                clicked = ImGui::Button(item ? item->name.c_str() : "Empty", buttonSize);
            }
            if (clicked && item) {
                clickedSlot = i;
            }

            ImGui::PopStyleVar();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    return clickedSlot;
}

void InventoryWindow::cleanup() {
    for (const auto& [path, texture] : iconTextures_) {
        glDeleteTextures(1, &texture);
    }
    iconTextures_.clear();
}

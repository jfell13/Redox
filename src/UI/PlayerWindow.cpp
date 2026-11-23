#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utility>
#include <Player.h>
#include <PlayerWindow.h>
#include <UIElements.h>

void PlayerWindow::playerWindow(Player& player) {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    const std::string windowTitle = "Player Window";
    if (ImGui::Begin(windowTitle.c_str(), &player.invOpen)) {
        constexpr int columns = 4;
        std::vector<bool> selected(player.currentInventory().slotCount(), false);
        if (ImGui::BeginTable(
            "Inventory Slots", columns,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame
                        )
            ) {
            for (std::size_t i = 0; i < player.currentInventory().slotCount(); ++i) {
                ImGui::TableNextColumn();
                const Item* item = player.currentInventory().itemAt(i);
                if (item) {
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Selectable(item->name.c_str())) {
                        Item transferredItem = *item;
                        transferredItem.stackSize = 1;
                        // if (player.addItem(transferredItem)) {
                        //     chest.removeItem(i);
                        // }
                    }
                    ImGui::PopID();
                } else {
                    ImGui::TextUnformatted("Empty");
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

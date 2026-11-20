#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <memory>
#include <span>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include <Model.h>
#include <Objects.h>
#include <Renderer.h>
#include <Controls.h>
#include <Item.h>
#include <Inventory.h>
#include <ItemCatalog.h>
#include <Player.h>
#include <MenuBar.h>
#include <LoggerWindow.h>

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "MyGame", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glfwSwapInterval(1);
    // Item Catalog Loading
    ItemCatalog catalog;
    catalog.load(ReDox_ITEMS_PATH);
    std::vector<Item> startingItems;
    startingItems.reserve(catalog.definitions().size());

    for (const auto& [id, definition] : catalog.definitions()) {
        Item item{};
        item.itemID = id;
        item.name = definition.name;
        item.itemWeight = definition.weight;
        item.maxStackSize = definition.maxStackSize;
        item.itemDescription = definition.description;
        item.stackSize = 1;  // Give the player one of each catalog item.

        startingItems.push_back(item);
    }
    // Player initiallization
    glm::vec3 spawnPoint = glm::vec3(15.0f, 15.0f, 0.5f);
    Player player(
        spawnPoint, 
        std::size_t(20),
        ReDox_MODEL_PATH,
        startingItems
    );
    
    const glm::vec3 particleColor(1.0f, 0.5f, 0.2f);
    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // ImGui Mouse Position Helper Function
    // ImVec2 mousePos = ImGui::GetMousePos();
    // Logger Window + Menu Bar
    LoggerWindow loggerWindow(20.0f, 20.0f, 260.0f, 120.0f);
    MenuBar menubar;
    // Renderer initialization
    Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Renderer initialization failed\n";
        renderer.cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Attempting to create containers loop for more than 1 "scene" container
    std::vector<std::unique_ptr<Container>> containers_;
    for (int i = 0; i <3; i++) {
        const glm::vec3& chestPosition = glm::vec3(i * 4.0f, i * 4.0f, 0.0f);
        std::vector<Item> contents(startingItems.begin(), startingItems.begin() + 1 + i);
        containers_.push_back(std::make_unique<Container>(
            chestPosition,
            i+4,
            ReDox_CRATE_PATH,
            contents
        ));
    }
    
    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwWindowShouldClose(window))
            break;

        const double now = glfwGetTime();
        // Limit jumps after a pause or a window drag.
        const float deltaTime = glm::clamp(static_cast<float>(now - previousTime), 0.0f, 0.1f);
        previousTime = now;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) {
            glfwWaitEvents();
            previousTime = glfwGetTime();
            continue;
        }

        // ImGui frame setup
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Text("Hello, world!");
        menubar.menuBar(window);
        if (ImGui::BeginViewportSideBar("StatusBar", nullptr, ImGuiDir_Down, 60.0f, ImGuiWindowFlags_NoDecoration)) {
            ImGui::Text("Application Status: Ready");
            ImGui::Text("Keyboard captured: %s",
                io.WantCaptureKeyboard ? "yes" : "no");
            ImGui::End();
        }
        if (ImGui::BeginViewportSideBar("Controls", nullptr, ImGuiDir_Up, 40.0f, ImGuiWindowFlags_NoDecoration)) {
            ImGui::Text("Simulation Controls.");
            ImGui::SameLine();
            if (ImGui::Button("Open")) { 
                loggerWindow.addMessage("You Clicked Open!");
            }
            ImGui::SameLine();
            if (ImGui::Button("Save")) {  
                loggerWindow.addMessage("You Clicked Save!");
            }
            ImGui::End();
        }
        // Framebuffer pixels also handle Retina displays and monitor changes.
        glViewport(0, 0, width, height);
        if (!io.WantCaptureKeyboard) {
            player.updateControls(window, deltaTime);
        }
        // Inventory Window
        if (!io.WantTextInput && ImGui::IsKeyPressed(ImGuiKey_I, false)) {
            player.invOpen = !player.invOpen;
        }
        if (player.invOpen) {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Inventory", &player.invOpen)) {
                if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0))) {
                    for (std::size_t i = 0; i < player.slotCount(); ++i) {
                        const Item* currItem = player.itemAt(i);
                        if (currItem == nullptr) {
                            ImGui::Text("Inventory slot %d: Empty", i + 1);
                        } else {
                            ImGui::Text("Inventory slot %d: %s", i + 1, currItem->name.c_str());
                        }
                    }
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
        const glm::mat4 view = player.viewMatrix();
        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), 
            static_cast<float>(width) / height, 
            0.1f, 
            100.0f);

        renderer.beginFrame(view, projection);
        renderer.drawXYPlane();

        for (const auto& chest : containers_) {
            renderer.drawModel(
                chest->model(), 
                chest->modelTransform(), 
                particleColor
            );
        }

        renderer.drawModel(
            player.model(), 
            player.modelTransform(), 
            particleColor
        );
        // Clicking on Chest; Opening Window
        float hitDistance = 0.0f;
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            glm::vec3 rayOrigin = glm::vec3(glm::inverse(view)[3]);
            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            glm::vec3 internalMousePos = player.GetRayFromMouse(
                mousePos.x, mousePos.y, windowWidth, 
                windowHeight, view, projection
            );
            // loggerWindow.addMessage(
            //     "Left Mouse Button Clicked at \nWindow: "
            //     + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y)
            //     + "\nRay: " + std::to_string(internalMousePos.x) + ", " 
            //     + std::to_string(internalMousePos.y) + ", " + std::to_string(internalMousePos.z)
            // );
            // for (auto& chest : containers_) {
            for (std::size_t index = 0; index < containers_.size(); ++index) {
                Container& chest = *containers_[index];
                
                if (chest.isClicked(rayOrigin, internalMousePos, hitDistance)) {
                    chest.invOpen = true;
                    loggerWindow.addMessage("Chest #" + std::to_string(index) + " was Clicked!");
                }
            }
        }
        
        for (std::size_t index = 0; index < containers_.size(); ++index) {
            Container& chest = *containers_[index];
            if (chest.invOpen) {
                ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
                const std::string windowTitle = "Chest #" + std::to_string(index) + " Inventory";
                if (ImGui::Begin(windowTitle.c_str(), &chest.invOpen)) {
                    constexpr int columns = 4;
                    std::vector<bool> selected(chest.slotCount(), false);
                    if (ImGui::BeginTable(
                        "ChestSlots", columns,
                                        ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame
                                    )
                        ) {
                        for (std::size_t i = 0; i < chest.slotCount(); ++i) {
                            ImGui::TableNextColumn();
                            const Item* item = chest.itemAt(i);
                            if (item) {
                                ImGui::PushID(static_cast<int>(i));
                                if (ImGui::Selectable(item->name.c_str())) {
                                    Item transferredItem = *item;
                                    transferredItem.stackSize = 1;

                                    if (player.addItem(transferredItem)) {
                                        chest.removeItem(i);
                                    }
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
        }
        // if (chestInvOpen) {
        //     ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        //     if (ImGui::Begin("Chest Inventory", &chestInvOpen)) {
        //         constexpr int columns = 4;
        //         std::vector<bool> selected(chest.slotCount(), false);
        //         if (ImGui::BeginTable(
        //             "ChestSlots", columns,
        //                             ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame
        //                         )
        //             ) {
        //             for (std::size_t i = 0; i < chest.slotCount(); ++i) {
        //                 ImGui::TableNextColumn();
        //                 const Item* item = chest.itemAt(i);
        //                 if (item) {
        //                     ImGui::PushID(static_cast<int>(i));
        //                     if (ImGui::Selectable(item->name.c_str())) {
        //                         Item transferredItem = *item;
        //                         transferredItem.stackSize = 1;

        //                         if (player.addItem(transferredItem)) {
        //                             chest.removeItem(i);
        //                         }
        //                     }
        //                     ImGui::PopID();

        //                 } else {
        //                     ImGui::TextUnformatted("Empty");
        //                 }
        //             }
        //             ImGui::EndTable();
        //         }
        //     }
        //     ImGui::End();
        // }
        loggerWindow.draw();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // OpenGL resources must be released while the context is still current.
    player.cleanup();
    for (auto& chest : containers_) {
        chest->cleanup();
    }
    renderer.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

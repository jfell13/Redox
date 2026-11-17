#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
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
    catalog.load("items.json");
    // if (const auto* potion = catalog.find("healing_potion")) {
    //     std::cout << potion->name << '\n';
    // }
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
    ImVec2 mousePos = ImGui::GetMousePos();
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

    Model chest;
    if (!chest.load("chest.obj")) {
        std::cerr << chest.error() << '\n';
        chest.cleanup();
        renderer.cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    const glm::vec3 chestCenter = (chest.minimum() + chest.maximum()) * 0.5f;
    const glm::vec3 chestOrigin(chestCenter.x, chestCenter.y, chest.minimum().z);
    constexpr float chestScale = 0.01f;
    ClickableModel clickableChest{&chest};

    Model model;
    if (!model.load(ReDox_MODEL_PATH)) {
        std::cerr << model.error() << '\n';
        model.cleanup();
        renderer.cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Center the imported geometry in XY and put its lowest Z on the ground.
    const glm::vec3 modelCenter = (model.minimum() + model.maximum()) * 0.5f;
    const glm::vec3 modelOrigin(modelCenter.x, modelCenter.y, model.minimum().z);
    constexpr float modelScale = 1.0f;
    std::cout << "Loaded character.obj; bounds: ("
              << model.minimum().x << ", " << model.minimum().y << ", " << model.minimum().z
              << ") to (" << model.maximum().x << ", " << model.maximum().y
              << ", " << model.maximum().z << ")\n";

    

    double previousTime = glfwGetTime();
    bool inventoryOpen = false;
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
            inventoryOpen = !inventoryOpen;
        }
        if (inventoryOpen) {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Inventory", &inventoryOpen)) {
                if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0))) {
                    int invCount = player.slotCount();
                    const Inventory& currInv = player.currentInventory();
                    for (int i = 0; i < invCount; ++i) {
                        const Item* currItem = currInv.itemAt(i);
                        if (currItem == nullptr) {
                            ImGui::Text("Inventory slot %d: Empty", i + 1);
                        } else {
                            ImGui::Text("Inventory slot %d: %s", i + 1, currItem->name.c_str());
                        }
                        // ImGui::Text("Inventory slot %d", i + 1);
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
        renderer.drawBox(glm::vec3(10.0f, 5.0f, 0.0f), glm::vec3(7.5f, 7.5f, 7.5f))
        .drawStairs();
        // Chest Test
        const glm::vec3& chestPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 chestTransform = glm::translate(
            glm::mat4(1.0f), 
            glm::vec3(
                0.0f, 
                0.0f, 
                0.0f)
            );
        chestTransform = glm::scale(
            chestTransform, 
            glm::vec3(chestScale)
        );
        renderer.drawModel(
            chest, 
            chestTransform, 
            particleColor
        );
        // Player Adjustments
        const glm::vec3& playerPosition = player.position();
        glm::mat4 modelTransform = glm::translate(
            glm::mat4(1.0f), 
            glm::vec3(
                playerPosition.x, 
                playerPosition.y, 
                0.0f)
            );
        modelTransform = glm::rotate(
            modelTransform,
            player.yaw(),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
        modelTransform = glm::scale(
            modelTransform, 
            glm::vec3(modelScale)
        );
        modelTransform = glm::translate(
            modelTransform, 
            -modelOrigin
        );
        renderer.drawModel(
            model, 
            modelTransform, 
            particleColor
        );
        // Testing Mouse Click
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            glm::vec3 rayOrigin = glm::vec3(glm::inverse(view)[3]);
            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            glm::vec3 internalMousePos = player.GetRayFromMouse(
                mousePos.x, 
                mousePos.y, 
                windowWidth, 
                windowHeight, 
                view, 
                projection
            );
            loggerWindow.addMessage(
                "Left Mouse Button Clicked at \nWindow: "
                + std::to_string(mousePos.x) 
                + ", " 
                + std::to_string(mousePos.y)
                + "\nRay: "
                + std::to_string(internalMousePos.x)
                + ", " 
                + std::to_string(internalMousePos.y)
                + ", " 
                + std::to_string(internalMousePos.z)
            );
            float hitDistance = 0.0f;
            clickableChest.transform = chestTransform;
            if (clickableChest.IsClicked(rayOrigin, internalMousePos, hitDistance)) {
                loggerWindow.addMessage(
                    "Chest was Clicked!"
                );
            }
        }
        
        // if (GLFW_MOUSE_BUTTON_LEFT)
        // if (!io.MouseClicked) {
        //         glm::vec3 internalMousePos = player.GetRayFromMouse(
        //             mousePos.x, 
        //             mousePos.x, 
        //             width, 
        //             height, 
        //             view, 
        //             projection);
        //         std::cout << "Mouse Screen Position: " << 
        //         mousePos.x <<
        //         ", " <<
        //         mousePos.y << "\n";
        //         std::cout << "Internal Mouse Position: " <<
        //         internalMousePos.x <<
        //         ", " <<
        //         internalMousePos.y << "\n";
        //         loggerWindow.addMessage(
        //             "You Clicked at " + std::to_string(mousePos.x) + ", " +
        //             std::to_string(mousePos.y)
        //         );
        //         if (internalMousePos == chestPosition) {
        //             loggerWindow.addMessage("You Clicked The Chest! (Internal)");
        //         }
                // if (mousePos == chestPosition) {
                //     loggerWindow.addMessage("You Clicked The Chest! (Internal)");
                // }

        // }
        
        loggerWindow.draw();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // OpenGL resources must be released while the context is still current.
    model.cleanup();
    chest.cleanup();
    renderer.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

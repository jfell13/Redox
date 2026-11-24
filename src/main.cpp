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
#include <Controls.h>
#include <Inventory.h>
#include <Item.h>
#include <ItemCatalog.h>
#include <LoggerWindow.h>
#include <MainMenu.h>
#include <MenuBar.h>
#include <Model.h>
#include <Objects.h>
#include <Player.h>
#include <PlayerWindow.h>
#include <Renderer.h>
#include <Scene.h>

enum States {
    MAINMENU,
    GAME,
    SETTINGS
};

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
    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    double previousTime = glfwGetTime();
    bool gameQuit = false;
    MainMenu mainmenu;
    States gameState = States::MAINMENU;
    // Item Catalog Loading
    ItemCatalog catalog;
    catalog.load(ReDox_ITEMS_PATH);
    std::vector<Item> startingItems;
    startingItems.reserve(catalog.definitions().size());
    for (const auto& [id, definition] : catalog.definitions()) {
        Item item{};
        item.itemID = id;
        item.name = definition.name;
        item.thumbnail = definition.thumbnail;
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
    PlayerWindow playerWindow;
    Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Renderer initialization failed\n";
        renderer.cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    LoggerWindow loggerWindow(20.0f, 20.0f, 260.0f, 120.0f);
    // Scene Initiialization
    Scene scene;
    scene.setStartingItems(startingItems);
    scene.initializeContainers(4, ReDox_CRATE_PATH);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwWindowShouldClose(window))
            break;
        if (gameQuit) {
            break;
        } 
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
        //Main Menu 
        if (gameState == States::MAINMENU) {
            mainmenu.draw();
            if (mainmenu.gameStart()) {
                gameState = States::GAME;
            }
            gameQuit = mainmenu.returnQuit();
        }
        // Game Loop
        else if (gameState == States::GAME) {
            //Menu Bar
            MenuBar menubar;
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
                playerWindow.playerWindow(player);
            }
            const glm::mat4 view = player.viewMatrix();
            const glm::mat4 projection = glm::perspective(
                glm::radians(45.0f), 
                static_cast<float>(width) / height, 
                0.1f, 
                100.0f);
            scene.beginFrame(renderer, player, width, height);
            scene.draw(renderer, player);
            // Clicking on Chest; Opening Window
            if (!io.WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                ImVec2 mousePos = ImGui::GetMousePos();
                glm::vec3 rayOrigin = glm::vec3(glm::inverse(view)[3]);
                int windowWidth = 0;
                int windowHeight = 0;
                glfwGetWindowSize(window, &windowWidth, &windowHeight);
                glm::vec3 rayDirection = player.GetRayFromMouse(
                    mousePos.x, mousePos.y, windowWidth, 
                    windowHeight, view, projection
                );
                scene.objectHit(rayOrigin, rayDirection);
            }

            if (scene.collisionDetection(player)) {
                loggerWindow.addMessage("COLLISION");
            }
            scene.containerWindow(player);
            
            loggerWindow.draw();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    player.cleanup();
    playerWindow.playerWindowCleanup();
    scene.sceneCleanup();
    renderer.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#include <Collisions.h>
#include <InventoryWindow.h>
#include <Item.h>
#include <ItemCatalog.h>
#include <Objects.h>
#include <Player.h>
#include <Renderer.h>
#include <Scene.h>
#include <algorithm>
#include <limits>
#include <memory>
#include <random>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <format>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>

std::pair<int ,int> windowSize(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return {width, height};
}

void Scene::setStartingItems(std::vector<Item> startingItems) {
    startingItems_ = startingItems;
}

void Scene::loadCatalog(std::string fileName) {
    catalog_.load(fileName);
}

bool Scene::initializeContainers(int containerPop, std::string fileName) {
    if (containerPop < 0) {
        return false;
    }

    const auto targetPopulation = static_cast<std::size_t>(containerPop);
    if (containers_.size() >= targetPopulation) {
        return true;
    }

    constexpr std::size_t inventoryCapacity = 12;
    constexpr int maxConsecutiveFailures = 5;
    const std::size_t maxStartingItems =
        std::min(startingItems_.size(), inventoryCapacity);

    std::uniform_real_distribution<float> distLengthX(0.0f, sceneSize_.x);
    std::uniform_real_distribution<float> distLengthY(0.0f, sceneSize_.y);
    // For an empty source list, this distribution is valid but never sampled.
    std::uniform_int_distribution<std::size_t> itemCount(
        1, std::max(std::size_t{1}, maxStartingItems));
    std::random_device rd;
    std::mt19937 gen(rd());
    int consecutiveFailures = 0;

    while (containers_.size() < targetPopulation &&
           consecutiveFailures < maxConsecutiveFailures) {
        const glm::vec3 containerPosition{
            distLengthX(gen),
            distLengthY(gen),
            0.0f
        };

        const bool overlaps = std::any_of(
            containers_.begin(), containers_.end(),
            [&containerPosition](const auto& container) {
                return glm::distance(containerPosition, container->position()) < 10.0f;
            });

        if (overlaps) {
            ++consecutiveFailures;
            continue;
        }

        std::vector<Item> contents = startingItems_;
        if (!contents.empty()) {
            std::shuffle(contents.begin(), contents.end(), gen);
            contents.resize(itemCount(gen));
        }

        containers_.push_back(std::make_unique<Container>(
            containerPosition, inventoryCapacity, fileName, contents));
        consecutiveFailures = 0;
    }

    return containers_.size() >= targetPopulation;
}

void Scene::beginFrame(Renderer& renderer, const Player& player, int width, int height) {
    const glm::mat4 view = player.viewMatrix();
    const glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        static_cast<float>(width) / height, 
        0.1f, 
        100.0f);
    renderer.beginFrame(view, projection);
}

void Scene::draw(Renderer& renderer, const Player& player) {
    renderer.drawXYPlane(glm::vec2(sceneSize_.x, sceneSize_.y));
    const glm::vec3 particleColor(1.0f, 0.5f, 0.2f);
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
}

void Scene::objectHit(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
    Container* nearestContainer = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();
    for (std::size_t index = 0; index < containers_.size(); ++index) {
        Container& chest = *containers_[index];
        float candidateDistance = 0.0f;
        if (chest.isClicked(rayOrigin, rayDirection, candidateDistance)) {
            if (candidateDistance < nearestDistance) {
                nearestDistance = candidateDistance;
                nearestContainer = &chest;
            }
        }
    }
    if (nearestContainer) {
        nearestContainer->invOpen = true;
    }
}

void Scene::containerWindow(Player& player) {
    for (std::size_t index = 0; index < containers_.size(); ++index) {
        Container& chest = *containers_[index];

        const auto clickedSlot = chest.showInventory(
            "Chest #" + std::to_string(index) + " Inventory"
        );

        if (clickedSlot) {
            const Item* item = chest.itemAt(*clickedSlot);

            if (item) {
                Item transferredItem = *item;
                transferredItem.stackSize = 1;

                if (player.addItem(transferredItem)) {
                    chest.removeItem(*clickedSlot);
                }
            }
        }
    }
}

void Scene::playerItemTransfer(Player& player, std::optional<std::size_t> clickedSlot) {
    if (!clickedSlot) { return; }
    for (std::size_t index = 0; index < containers_.size(); ++index) { 
        Container& chest = *containers_[index];
        if (chest.invOpen) {
            const Item* item = player.itemAt(*clickedSlot);
            if (item) {
                Item transferredItem = *item;
                transferredItem.stackSize = 1;
                if (chest.addItem(transferredItem)) {
                    player.removeItem(*clickedSlot);
                    return;
                }
            }
        }
    }
}

bool Scene::collisionDetection(Player& player) {
    const AABB& playerAABB = player.model().getWorldAABB(
        player.model().modelBounds(),
        player.modelTransform()
    );
    for (const auto& chest : containers_) {
        const AABB modelAABB = chest->model().getWorldAABB(
            chest->model().modelBounds(),
            chest->modelTransform()
        );
        bool collision = checkOverlap(playerAABB, modelAABB);
        if (collision) {
            return true;
        }
    }
    return false;
}

void Scene::resolveCollision(Player& player, glm::vec3 previousPosition) {
    glm::vec3 movement = player.position() - previousPosition;
    glm::vec3 accepted = previousPosition;
    // X-axis 
    glm::vec3 xCandidate = accepted + glm::vec3(movement.x, 0.0f, 0.0f);
    player.setPosition(xCandidate);
    if (!collisionDetection(player)) {
        accepted = xCandidate;
    }
    // Y-axis
    glm::vec3 yCandidate = accepted + glm::vec3(0.0f, movement.y, 0.0f);
    player.setPosition(yCandidate);
    if (!collisionDetection(player)) {
        accepted = yCandidate;
    }
    // Z-axis
    glm::vec3 zCandidate = accepted + glm::vec3(0.0f, 0.0f, movement.z);
    player.setPosition(zCandidate);
    if (!collisionDetection(player)) {
        accepted = zCandidate;
    }
    // set collision adjusted position
    player.setPosition(accepted);
}

void Scene::sceneCleanup() {
    for (auto& chest : containers_) {
        chest->cleanup();
    }
}


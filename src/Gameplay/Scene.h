#ifndef SCENE_H
#define SCENE_H
#include <array>
#include <memory>
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <Inventory.h>
#include <ItemCatalog.h>
#include <Objects.h>
#include <Player.h>
#include <Renderer.h>

class Scene{
    public:
        void setStartingItems(std::vector<Item> startingItems);
        void loadCatalog(std::string fileName);
        bool initializeContainers(int containerPop, std::string fileName);
        const glm::vec3& sceneSize() const { return sceneSize_; }
        const std::unordered_map<std::string, GLuint>& icons() const { return iconTextures_; }
        void beginFrame(Renderer& renderer, const Player& player, int width, int height);
        void draw(Renderer& renderer, const Player& player);
        void objectHit(glm::vec3 rayOrigin, glm::vec3 rayDirection);
        void containerWindow(Player& player);
        void playerItemTransfer(Player& player, std::optional<std::size_t> clickedSlot);
        bool collisionDetection(Player& player);
        void resolveCollision(Player& player, glm::vec3 previousPosition);
        void sceneCleanup();
    private:
        const glm::vec3 sceneSize_ = glm::vec3(100.0f, 100.0f, 50.0f);
        // std::mt19937 gen_;
        std::vector<Item> startingItems_;
        ItemCatalog catalog_;
        std::unordered_map<std::string, GLuint> iconTextures_;
        std::vector<std::unique_ptr<Container>> containers_;

};




#endif
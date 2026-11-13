#ifndef PLAYER_H
#define PLAYER_H
#include <Controls.h>
#include <Inventory.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <map>


class Player {
    public:
        explicit Player(
            const glm::vec3& initialPosition,
            const std::unordered_map<ItemId, int>& initialInventory,
            const std::unordered_map<std::string, int>& initialAttributes,
            const std::unordered_map<std::string, int>& initialSkills,
            const std::unordered_map<std::string, int>& initialMerits,
            const std::unordered_map<std::string, int>& iniitalFlaws
        );
        const glm::vec3& position() const { return position_; }
    
    private:
        glm::vec3 position_;
        PlayerControls controls_;
        Inventory inventory_;
        std::unordered_map<std::string, int> attributes_;
        std::unordered_map<std::string, int> skills_;
        std::unordered_map<std::string, int> merits_;
        std::unordered_map<std::string, int> flaws_;

};

#endif
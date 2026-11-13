#include <Player.h>
#include <Controls.h>
#include <Inventory.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <map>

Player::Player(
    const glm::vec3& initialPosition,
    const std::unordered_map<ItemId, int>& initialInventory,
    const std::unordered_map<std::string, int>& initialAttributes,
    const std::unordered_map<std::string, int>& initialSkills,
    const std::unordered_map<std::string, int>& initialMerits,
    const std::unordered_map<std::string, int>& iniitalFlaws
) 
    : position_(initialPosition),
    inventory_(initialInventory),
    attributes_(initialAttributes),
    skills_(initialSkills),
    merits_(initialMerits),
    flaws_(iniitalFlaws),
    controls_(initialPosition) {}


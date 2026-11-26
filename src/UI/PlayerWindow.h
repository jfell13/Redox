#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H
#include <Player.h>
#include "InventoryWindow.h"
#include <glad/gl.h>
#include <unordered_map>
#include <string>

class PlayerWindow{
    public:
        std::optional<std::size_t> playerWindow(Player& player);
        void playerWindowCleanup(); 
    private:
        InventoryWindow inventoryWindow_;
        std::unordered_map<std::string, GLuint> iconTextures_;

};

#endif
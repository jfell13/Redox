#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H
#include <Player.h>
#include <glad/gl.h>
#include <unordered_map>
#include <string>

class PlayerWindow{
    public:
         void playerWindow(Player& player);
         void playerWindowCleanup();
    private:
        std::unordered_map<std::string, GLuint> iconTextures_;

};

#endif
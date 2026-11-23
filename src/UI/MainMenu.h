#ifndef MAINMENU_H
#define MAINMENU_H
#include <UIElements.h>
#include <vector>
#include <string>
#include <deque>
#include <utility>
#include <glm/glm.hpp>

class MainMenu{
    public:
        void configureMainMenu();
        void draw();
        bool gameStart() {return Game_;}
        bool returnQuit() {return Quit_;}
    private:
        bool Quit_ = false;
        bool Game_ = false;
};

#endif
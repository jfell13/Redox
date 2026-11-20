#ifndef LOGGERWINDOW_H
#define LOGGERWINDOW_H
#include <vector>
#include <string>
#include <deque>
#include <utility>
#include <glm/glm.hpp>

struct LogEntry {
    std::string message;
    // Later: timestamp, severity, color...
};

enum elementType {
	GUI_TEXT,
	GUI_SLIDER,
	GUI_BUTTON
};

enum textSide {
	GUI_LEFT_SIDE,
	GUI_CENTER_SIDE,
	GUI_RIGHT_SIDE
};

struct GUIelement {
	elementType type;
	std::string string;
	double charSize;
	double* data;
	double min;
	double max;
	int side;
	glm::vec3 textColor;
	glm::vec3 backgroundColor;
};

struct LoggerWindow {
    float xStreamPos;
    float yStreamPos;
    float xStreamSize;
    float yStreamSize;
    // Constructor
    LoggerWindow(
        float init_xStreamPos,
        float init_yStreamPos,
        float init_xStreamSize,
        float init_yStreamSize
    );
    // Methods
    std::vector<GUIelement> elements;

    void configureLogWindow();
    void addText(const char* str, double charSize, int side, glm::vec3 textCol);
    void addMessage(std::string message);
    void draw();

    private:
        std::deque<LogEntry> entries;
        std::size_t maxEntries = 1000;
};

#endif 
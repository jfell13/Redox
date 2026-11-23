#ifndef UIELEMENTS_H
#define UIELEMENTS_H
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

#endif
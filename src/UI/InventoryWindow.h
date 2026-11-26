#ifndef INVENTORYWINDOW_H
#define INVENTORYWINDOW_H

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <glad/gl.h>

class Inventory;

struct InventoryWindow {
    std::optional<std::size_t> window(
        const Inventory& inventory, const std::string& inventoryName,
        bool* open = nullptr);
    std::optional<std::size_t> drawContents(const Inventory& inventory);
    void cleanup();

private:
    std::unordered_map<std::string, GLuint> iconTextures_;
};

#endif

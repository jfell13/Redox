#ifndef ITEMCATALOG_H
#define ITEMCATALOG_H
#include <filesystem>
#include <boost/json.hpp>
#include <unordered_map>
#include <string>

struct ItemDefinition {
    std::string name;
    float weight;
    int maxStackSize;
    std::string description;
};

class ItemCatalog {
    public:
        void load(const std::filesystem::path& path);
        const ItemDefinition* find(const std::string& id) const;
        const auto& definitions() const {
            return definitions_;
        }
    private:
        std::unordered_map<std::string, ItemDefinition> definitions_;
    };

#endif
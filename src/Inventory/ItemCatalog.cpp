#include <ItemCatalog.h>
#include <boost/json.hpp>
#include <iomanip>
#include <iostream>
#include <fstream>

namespace json = boost::json;

void ItemCatalog::load(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open " + path.string());
    }

    const json::value data = json::parse(file);
    decltype(definitions_) loaded;

    for (const auto& entry : data.as_array()) {
        const auto& record = entry.as_object();
        const auto id = json::value_to<std::string>(record.at("id"));

        ItemDefinition definition{
            json::value_to<std::string>(record.at("name")),
            json::value_to<std::string>(record.at("thumbnail")),
            json::value_to<float>(record.at("weight")),
            json::value_to<int>(record.at("maxStackSize")),
            json::value_to<std::string>(record.at("description"))
        };

        if (!loaded.emplace(id, std::move(definition)).second) {
            throw std::runtime_error("Duplicate item ID in " + path.string() + ": " + id);
        }
    }

    definitions_.swap(loaded);
}

const ItemDefinition* ItemCatalog::find(const std::string& id) const {
    const auto found = definitions_.find(id);
    return found == definitions_.end() ? nullptr : &found->second;
}
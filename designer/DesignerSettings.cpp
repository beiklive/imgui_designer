#include "DesignerSettings.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace designer {
bool DesignerSettings::load(const std::filesystem::path& path) {
    std::ifstream stream(path);
    if (!stream) return false;
    try {
        nlohmann::json json; stream >> json;
        windowWidth = json.value("windowWidth", windowWidth);
        windowHeight = json.value("windowHeight", windowHeight);
        language = json.value("language", std::string("en")) == "zh-CN" ? ui::Language::Chinese : ui::Language::English;
        lastDocument = json.value("lastDocument", lastDocument);
        return true;
    } catch (...) { return false; }
}

bool DesignerSettings::save(const std::filesystem::path& path) const {
    std::ofstream stream(path);
    if (!stream) return false;
    nlohmann::json json{
        {"windowWidth", windowWidth}, {"windowHeight", windowHeight},
        {"language", language == ui::Language::Chinese ? "zh-CN" : "en"},
        {"lastDocument", lastDocument}
    };
    stream << json.dump(2) << '\n';
    return true;
}
}

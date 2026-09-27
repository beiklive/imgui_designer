#pragma once
#include "UIDocument.h"
#include <nlohmann/json.hpp>
#include <string>

namespace ui {
class UIParser {
public:
    UIDocument parse(const nlohmann::json& json) const;
    UIDocument load(const std::string& path) const;
private:
    std::unique_ptr<UIElement> parseElement(const nlohmann::json& json, const std::string& baseDirectory) const;
};
}

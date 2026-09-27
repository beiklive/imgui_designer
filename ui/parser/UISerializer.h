#pragma once
#include "../core/UIElement.h"
#include <nlohmann/json.hpp>
#include <string>

namespace ui {
class UISerializer {
public:
    nlohmann::json serialize(const UIElement& root, int version = 1) const;
    void save(const UIElement& root, const std::string& path, int version = 1) const;
private:
    nlohmann::json serializeElement(const UIElement& element) const;
};
}

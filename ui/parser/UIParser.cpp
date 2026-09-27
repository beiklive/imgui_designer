#include "UIParser.h"
#include "../widgets/Box.h"
#include "../widgets/Text.h"
#include "../widgets/Image.h"
#include "../containers/ScrollContainer.h"
#include "../layout/UISpring.h"
#include <fstream>
#include <stdexcept>

namespace ui {
static UIColor colorFromJson(const nlohmann::json& j, UIColor fallback) {
    if (!j.is_string()) return fallback;
    std::string s = j.get<std::string>();
    if (s.size() != 7 && s.size() != 9) return fallback;
    try {
        const auto channel = [&](size_t pos) { return static_cast<float>(std::stoul(s.substr(pos, 2), nullptr, 16)) / 255.0f; };
        return {channel(1), channel(3), channel(5), s.size() == 9 ? channel(7) : 1.0f};
    } catch (...) { return fallback; }
}

UIDocument UIParser::parse(const nlohmann::json& json) const {
    if (!json.is_object() || !json.contains("root")) throw std::runtime_error("UI document requires a root element");
    UIDocument document;
    document.version = json.value("version", 1);
    document.root = parseElement(json.at("root"));
    return document;
}
UIDocument UIParser::load(const std::string& path) const {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open UI document: " + path);
    nlohmann::json json; file >> json;
    return parse(json);
}
std::unique_ptr<UIElement> UIParser::parseElement(const nlohmann::json& j) const {
    const auto type = j.value("type", std::string("Box"));
    if (type != "Box" && type != "Text" && type != "Image" && type != "HorizontalSpring" && type != "VerticalSpring" && type != "VerticalScroll" && type != "HorizontalScroll")
        throw std::runtime_error("Unsupported element type: " + type);
    std::unique_ptr<UIElement> e;
    if (type == "Box") e = std::make_unique<Box>();
    else if (type == "Text") e = std::make_unique<Text>();
    else if (type == "Image") e = std::make_unique<Image>();
    else if (type == "HorizontalSpring") e = std::make_unique<HorizontalSpring>();
    else if (type == "VerticalSpring") e = std::make_unique<VerticalSpring>();
    else if (type == "VerticalScroll") e = std::make_unique<VerticalScroll>();
    else e = std::make_unique<HorizontalScroll>();
    e->id = j.value("id", std::string{});
    e->text = j.value("text", std::string{});
    e->source = j.value("source", std::string{});
    e->fit = j.value("fit", std::string("contain"));
    e->horizontalAlignment = j.value("horizontalAlignment", std::string("left"));
    e->verticalAlignment = j.value("verticalAlignment", std::string("top"));
    e->fontSize = j.value("fontSize", 18.0f);
    if (j.contains("layout")) {
        const auto& l = j.at("layout");
        const auto mode = l.value("mode", std::string("absolute"));
        if (mode == "horizontal") e->layout.mode = LayoutMode::Horizontal;
        else if (mode == "vertical") e->layout.mode = LayoutMode::Vertical;
        else if (mode == "overlay") e->layout.mode = LayoutMode::Overlay;
        else if (mode != "absolute") throw std::runtime_error("Unknown layout mode: " + mode);
        e->layout.x = l.value("x", 0.0f); e->layout.y = l.value("y", 0.0f);
        e->layout.width = l.value("width", 100.0f); e->layout.height = l.value("height", 40.0f);
        e->layout.spacing = l.value("spacing", 0.0f);
    }
    if (j.contains("style")) {
        const auto& s = j.at("style");
        e->style.background = colorFromJson(s.value("background", nlohmann::json{}), e->style.background);
        e->style.borderColor = colorFromJson(s.value("border", nlohmann::json{}), e->style.borderColor);
        e->style.textColor = colorFromJson(s.value("textColor", nlohmann::json{}), e->style.textColor);
        e->style.opacity = s.value("opacity", 1.0f); e->style.borderWidth = s.value("borderWidth", 0.0f);
        e->style.radius = s.value("radius", 0.0f); e->style.shadow = s.value("shadow", 0.0f);
        e->style.clip = s.value("clip", false);
        e->style.paddingLeft = s.value("paddingLeft", 0.0f); e->style.paddingRight = s.value("paddingRight", 0.0f);
        e->style.paddingTop = s.value("paddingTop", 0.0f); e->style.paddingBottom = s.value("paddingBottom", 0.0f);
    }
    if (j.contains("children")) for (const auto& child : j.at("children")) e->addChild(parseElement(child));
    return e;
}
}

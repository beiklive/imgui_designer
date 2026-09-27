#include "UISerializer.h"
#include <fstream>

namespace ui {
static nlohmann::json colorJson(UIColor c) {
    auto hex = [](float f) { const int n = static_cast<int>(f * 255.0f + 0.5f); const char* digits = "0123456789ABCDEF"; std::string s; s += digits[(n >> 4) & 15]; s += digits[n & 15]; return s; };
    std::string value="#" + hex(c.r) + hex(c.g) + hex(c.b);
    if (c.a < 0.999f) value += hex(c.a);
    return value;
}
nlohmann::json UISerializer::serialize(const UIElement& root, int version) const { return {{"version", version}, {"root", serializeElement(root)}}; }
void UISerializer::save(const UIElement& root, const std::string& path, int version) const {
    std::ofstream file(path); if (!file) throw std::runtime_error("Cannot write UI document: " + path);
    file << serialize(root, version).dump(2) << '\n';
}
nlohmann::json UISerializer::serializeElement(const UIElement& e) const {
    const char* mode = e.layout.mode == LayoutMode::Horizontal ? "horizontal" : e.layout.mode == LayoutMode::Vertical ? "vertical" : e.layout.mode == LayoutMode::Overlay ? "overlay" : "absolute";
    nlohmann::json j = {{"type", e.type}, {"id", e.id}, {"layout", {{"mode", mode}, {"x", e.layout.x}, {"y", e.layout.y}, {"width", e.layout.width}, {"height", e.layout.height}, {"spacing", e.layout.spacing}}},
        {"style", {{"background", colorJson(e.style.background)}, {"border", colorJson(e.style.borderColor)}, {"textColor", colorJson(e.style.textColor)}, {"opacity", e.style.opacity}, {"borderWidth", e.style.borderWidth}, {"radius", e.style.radius}, {"shadow", e.style.shadow}, {"clip", e.style.clip}, {"paddingLeft", e.style.paddingLeft}, {"paddingRight", e.style.paddingRight}, {"paddingTop", e.style.paddingTop}, {"paddingBottom", e.style.paddingBottom}}}};
    if (!e.text.empty() || e.type == "Text") j["text"] = e.text;
    if (!e.source.empty() || e.type == "Image") j["source"] = e.source;
    if (e.type == "Image") j["fit"] = e.fit;
    if (e.type == "Text") { j["fontSize"] = e.fontSize; j["horizontalAlignment"] = e.horizontalAlignment; j["verticalAlignment"] = e.verticalAlignment; }
    if (!e.children().empty()) { j["children"] = nlohmann::json::array(); for (const auto& c : e.children()) j["children"].push_back(serializeElement(*c)); }
    return j;
}
}

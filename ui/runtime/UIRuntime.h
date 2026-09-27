#pragma once
#include "../parser/UIParser.h"
#include "../layout/UILayoutEngine.h"
#include "../renderer/UIRenderer.h"

namespace ui {
class UIRuntime {
public:
    explicit UIRuntime(UIParser parser = {}) : parser_(std::move(parser)) {}
    void load(const std::string& path);
    void setDocument(UIDocument document) { document_ = std::move(document); }
    UIDocument& document() { return document_; }
    const UIDocument& document() const { return document_; }
    void update(float deltaTime);
    void layout(UIRect bounds);
    void render(UIRenderer& renderer, UIRect viewport, float scale = 1);
private:
    UIParser parser_;
    UIDocument document_;
    UILayoutEngine layout_;
};
}

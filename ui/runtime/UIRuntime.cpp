#include "UIRuntime.h"
#include <stdexcept>

namespace ui {
void UIRuntime::load(const std::string& path) { document_ = parser_.load(path); }
void UIRuntime::update(float deltaTime) { if (document_.root) document_.root->update(deltaTime); }
void UIRuntime::layout(UIRect bounds) { if (document_.root) layout_.layout(*document_.root, bounds); }
void UIRuntime::render(UIRenderer& renderer, UIRect viewport, float scale) {
    if (!document_.root) throw std::runtime_error("No UI document is loaded");
    renderer.beginFrame(viewport, scale); renderer.render(*document_.root); renderer.endFrame();
}
}

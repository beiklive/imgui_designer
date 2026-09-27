#pragma once
#include "../core/UIElement.h"
#include "UIRenderContext.h"

namespace ui {
class UIRenderer {
public:
    virtual ~UIRenderer() = default;
    virtual void beginFrame(UIRect viewport, float scale = 1.0f) = 0;
    virtual void render(UIElement& root) = 0;
    virtual void endFrame() = 0;
};
}

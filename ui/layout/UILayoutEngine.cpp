#include "UILayoutEngine.h"
#include <algorithm>
#include <vector>

namespace ui {
void UILayoutEngine::layout(UIElement& root, UIRect bounds) const { layoutElement(root, bounds); }

void UILayoutEngine::layoutElement(UIElement& e, UIRect rect) const {
    e.computedRect = rect;
    const float left = e.style.paddingLeft, right = e.style.paddingRight;
    const float top = e.style.paddingTop, bottom = e.style.paddingBottom;
    UIRect inner{rect.x + left, rect.y + top, std::max(0.0f, rect.width - left - right), std::max(0.0f, rect.height - top - bottom)};
    auto& children = e.children();
    if (children.empty()) { e.contentRect = inner; return; }

    if (e.scrollable) {
        float cursor = 0, extent = 0;
        for (const auto& c : children) {
            const float size = e.horizontalScroll ? c->layout.width : c->layout.height;
            UIRect childRect = e.horizontalScroll
                ? UIRect{inner.x + cursor - e.scrollX, inner.y, c->layout.width, inner.height}
                : UIRect{inner.x, inner.y + cursor - e.scrollY, inner.width, c->layout.height};
            layoutElement(*c, childRect);
            cursor += size + e.layout.spacing;
            extent += size + e.layout.spacing;
        }
        if (!children.empty()) extent -= e.layout.spacing;
        e.contentRect = e.horizontalScroll ? UIRect{inner.x, inner.y, extent, inner.height} : UIRect{inner.x, inner.y, inner.width, extent};
        return;
    }

    switch (e.layout.mode) {
    case LayoutMode::Absolute:
        for (const auto& c : children)
            layoutElement(*c, {inner.x + c->layout.x, inner.y + c->layout.y, c->layout.width, c->layout.height});
        break;
    case LayoutMode::Overlay:
        for (const auto& c : children)
            layoutElement(*c, {inner.x + c->layout.x, inner.y + c->layout.y,
                c->layout.width > 0 ? c->layout.width : inner.width,
                c->layout.height > 0 ? c->layout.height : inner.height});
        break;
    case LayoutMode::Horizontal:
    case LayoutMode::Vertical: {
        const bool horizontal = e.layout.mode == LayoutMode::Horizontal;
        float available = (horizontal ? inner.width : inner.height) - e.layout.spacing * static_cast<float>(children.size() - 1);
        float fixed = 0;
        size_t springs = 0;
        for (const auto& c : children) {
            const bool spring = horizontal ? c->horizontalSpring : c->verticalSpring;
            if (spring) ++springs;
            else fixed += horizontal ? c->layout.width : c->layout.height;
        }
        const float springSize = springs ? std::max(0.0f, available - fixed) / static_cast<float>(springs) : 0;
        float cursor = horizontal ? inner.x : inner.y;
        for (const auto& c : children) {
            const bool spring = horizontal ? c->horizontalSpring : c->verticalSpring;
            const float main = spring ? springSize : (horizontal ? c->layout.width : c->layout.height);
            UIRect childRect = horizontal
                ? UIRect{cursor, inner.y, main, c->layout.height > 0 ? c->layout.height : inner.height}
                : UIRect{inner.x, cursor, c->layout.width > 0 ? c->layout.width : inner.width, main};
            layoutElement(*c, childRect);
            cursor += main + e.layout.spacing;
        }
        break;
    }
    }
    e.contentRect = inner;
}
}

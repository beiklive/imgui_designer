#pragma once
#include "../core/UIElement.h"

namespace ui {
class UILayoutEngine {
public:
    void layout(UIElement& root, UIRect bounds) const;
private:
    void layoutElement(UIElement& element, UIRect rect) const;
};
}

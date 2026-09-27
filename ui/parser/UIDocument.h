#pragma once
#include "../core/UIElement.h"
#include <memory>

namespace ui {
struct UIDocument {
    int version = 1;
    std::unique_ptr<UIElement> root;
};
}

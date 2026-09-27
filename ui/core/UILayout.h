#pragma once

namespace ui {
enum class LayoutMode { Absolute, Horizontal, Vertical, Overlay };
struct UILayout {
    LayoutMode mode = LayoutMode::Absolute;
    float x = 0, y = 0, width = 100, height = 40;
    float spacing = 0;
};
}

#pragma once
#include "UIRect.h"

namespace ui {
struct UIStyle {
    UIColor background{0.12f, 0.14f, 0.18f, 1};
    UIColor borderColor{0.28f, 0.32f, 0.38f, 1};
    UIColor textColor{0.92f, 0.94f, 0.97f, 1};
    float opacity = 1, borderWidth = 0, radius = 0, shadow = 0;
    float paddingLeft = 0, paddingRight = 0, paddingTop = 0, paddingBottom = 0;
    bool clip = false;
};
}

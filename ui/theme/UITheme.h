#pragma once
#include "../core/UIStyle.h"

namespace ui {
struct UITheme {
    UIColor canvas{0.10f, 0.11f, 0.13f, 1};
    UIColor panel{0.13f, 0.14f, 0.17f, 1};
    UIColor accent{0.22f, 0.58f, 0.95f, 1};
    float spacing = 8, radius = 4;
    UIStyle defaultBox{};
};
}

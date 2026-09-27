#pragma once
#include "../core/UIElement.h"
namespace ui {
class HorizontalSpring final : public UIElement { public: HorizontalSpring() : UIElement("HorizontalSpring") { horizontalSpring=true; layout.width=0; layout.height=0; } };
class VerticalSpring final : public UIElement { public: VerticalSpring() : UIElement("VerticalSpring") { verticalSpring=true; layout.width=0; layout.height=0; } };
}

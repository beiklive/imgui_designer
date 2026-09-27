#pragma once
#include "../core/UIElement.h"

namespace ui {
class Label final : public UIElement {
public:
    Label() : UIElement("Label") {}
};
}

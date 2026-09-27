#pragma once
#include "UIElement.h"
namespace ui {
class UIContainer : public UIElement {
public:
    explicit UIContainer(std::string type = "Box") : UIElement(std::move(type)) {}
};
}

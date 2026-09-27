#include "UIElement.h"
#include <algorithm>

namespace ui {
UIElement::UIElement(std::string elementType) : type(std::move(elementType)) {}
void UIElement::update(float deltaTime) {
    for (auto& child : children_) child->update(deltaTime);
}
UIElement& UIElement::addChild(std::unique_ptr<UIElement> child) {
    child->parent_ = this;
    children_.push_back(std::move(child));
    return *children_.back();
}
bool UIElement::removeChild(UIElement* child) {
    const auto it = std::find_if(children_.begin(), children_.end(), [child](const auto& item) { return item.get() == child; });
    if (it == children_.end()) return false;
    (*it)->parent_ = nullptr;
    children_.erase(it);
    return true;
}
}

#pragma once
#include "UILayout.h"
#include "UIRect.h"
#include "UIState.h"
#include "UIStyle.h"
#include <memory>
#include <string>
#include <vector>

namespace ui {
class UIElement {
public:
    explicit UIElement(std::string elementType = "Box");
    virtual ~UIElement() = default;
    UIElement(const UIElement&) = delete;
    UIElement& operator=(const UIElement&) = delete;

    void update(float deltaTime);
    UIElement* parent() const { return parent_; }
    UIElement& addChild(std::unique_ptr<UIElement> child);
    bool removeChild(UIElement* child);
    void clearChildren();
    const std::vector<std::unique_ptr<UIElement>>& children() const { return children_; }

    std::string id;
    std::string type;
    UILayout layout;
    UIStyle style;
    UIState state;
    UIRect computedRect;
    std::string text;
    std::string source;
    std::string reference;
    std::string fit = "contain";
    std::string horizontalAlignment = "left";
    std::string verticalAlignment = "top";
    float fontSize = 18;
    bool horizontalSpring = false;
    bool verticalSpring = false;
    bool scrollable = false;
    bool horizontalScroll = false;
    float scrollX = 0, scrollY = 0;
    UIRect contentRect;

private:
    UIElement* parent_ = nullptr;
    std::vector<std::unique_ptr<UIElement>> children_;
};
}

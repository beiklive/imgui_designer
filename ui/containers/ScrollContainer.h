#pragma once
#include "../core/UIContainer.h"
namespace ui {
class ScrollContainer : public UIContainer { public: explicit ScrollContainer(std::string type, bool horizontal) : UIContainer(std::move(type)) { scrollable=true; horizontalScroll=horizontal; style.clip=true; } };
class VerticalScroll final : public ScrollContainer { public: VerticalScroll() : ScrollContainer("VerticalScroll",false) {} };
class HorizontalScroll final : public ScrollContainer { public: HorizontalScroll() : ScrollContainer("HorizontalScroll",true) {} };
}

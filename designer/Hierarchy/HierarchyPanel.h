#pragma once
#include "../../ui/core/UIElement.h"
#include "../../ui/i18n/Localization.h"
namespace designer { class HierarchyPanel { public: void draw(ui::UIElement& root, ui::UIElement*& selection, bool& deleteRequested, ui::Localization& localization); private: void drawElement(ui::UIElement& element, ui::UIElement*& selection, bool& deleteRequested, ui::Localization& localization); }; }

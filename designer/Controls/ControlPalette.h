#pragma once
#include "../../ui/core/UIElement.h"
#include "../../ui/i18n/Localization.h"
#include <string>

namespace designer {
class ControlPalette {
public:
    void draw(ui::UIElement*& selection, std::string& importPath, bool& newCanvas, bool& importComponent, ui::Localization& localization);

private:
    ui::UIElement* addElement(ui::UIElement& root, const std::string& type);
    unsigned int idCounter_ = 1;
};
}

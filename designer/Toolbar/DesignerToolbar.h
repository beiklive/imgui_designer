#pragma once
#include "../../ui/i18n/Localization.h"
#include <string>
namespace designer { class DesignerToolbar { public: void draw(float& zoom, std::string& path, bool& load, bool& save, ui::Localization& localization); }; }

#pragma once
#include "../ui/i18n/Localization.h"
#include <filesystem>
#include <string>

namespace designer {
struct DesignerSettings {
    int windowWidth = 1440;
    int windowHeight = 900;
    ui::Language language = ui::Language::English;
    std::string lastDocument;

    bool load(const std::filesystem::path& path);
    bool save(const std::filesystem::path& path) const;
};
}

#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ui {
enum class Language { English = 0, Chinese = 1 };

class Localization {
public:
    Localization();

    bool loadDirectory(const std::filesystem::path& directory);
    bool loadLanguageFile(Language language, const std::filesystem::path& file);

    void setLanguage(Language language) { language_ = language; }
    Language language() const { return language_; }
    std::string languageCode() const;
    std::string languageName(Language language) const;
    std::string tr(std::string_view key) const;

private:
    using Catalog = std::unordered_map<std::string, std::string>;
    static size_t index(Language language) { return static_cast<size_t>(language); }
    void installDefaults();

    Catalog catalogs_[2];
    Language language_ = Language::English;
};
}

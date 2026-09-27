#include "DesignerToolbar.h"
#include <imgui.h>

namespace designer {
void DesignerToolbar::draw(float& zoom, std::string& path, bool& load, bool& save, bool& running, ui::Localization& localization) {
    const auto file = localization.tr("menu.file");
    const auto loadLabel = localization.tr("toolbar.load");
    const auto saveLabel = localization.tr("toolbar.save");
    const auto fitLabel = localization.tr("toolbar.fit");
    const auto languageLabel = localization.tr("toolbar.language");
    const auto runLabel = localization.tr(running ? "tabs.pause" : "tabs.run");
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu(file.c_str())) {
        if (ImGui::MenuItem(localization.tr("menu.load").c_str())) load=true;
        if (ImGui::MenuItem(localization.tr("menu.save").c_str(), "Ctrl+S")) save=true;
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu(localization.tr("menu.settings").c_str())) {
        if (ImGui::MenuItem(localization.tr("settings.language").c_str())) {}
        if (ImGui::MenuItem(localization.languageName(ui::Language::English).c_str(), nullptr, localization.language() == ui::Language::English)) localization.setLanguage(ui::Language::English);
        if (ImGui::MenuItem(localization.languageName(ui::Language::Chinese).c_str(), nullptr, localization.language() == ui::Language::Chinese)) localization.setLanguage(ui::Language::Chinese);
        ImGui::EndMenu();
    }
    ImGui::TextUnformatted(localization.tr("app.title").c_str());
    ImGui::SameLine();
    static char pathBuffer[512]{};
    static std::string lastPath;
    if (lastPath != path) { std::snprintf(pathBuffer,sizeof(pathBuffer),"%s",path.c_str()); lastPath = path; }
    ImGui::SetNextItemWidth(340);
    if (ImGui::InputText("##documentPath",pathBuffer,sizeof(pathBuffer))) path=pathBuffer;
    if (ImGui::Button(loadLabel.c_str())) load=true; ImGui::SameLine();
    if (ImGui::Button(saveLabel.c_str())) save=true; ImGui::SameLine();
    if (ImGui::Button("-")) zoom=std::max(.1f,zoom-.1f); ImGui::SameLine();
    if (ImGui::Button(fitLabel.c_str())) zoom=-1.0f; ImGui::SameLine();
    if (ImGui::Button("+")) zoom=std::min(3.0f,zoom+.1f); ImGui::SameLine();
    if (ImGui::Button(runLabel.c_str())) running = !running; ImGui::SameLine();
    ImGui::TextUnformatted(languageLabel.c_str()); ImGui::SameLine();
    int language = localization.language() == ui::Language::Chinese ? 1 : 0;
    const std::string languageEnglish = localization.languageName(ui::Language::English);
    const std::string languageChinese = localization.languageName(ui::Language::Chinese);
    const char* languageOptions[]{languageEnglish.c_str(), languageChinese.c_str()};
    ImGui::SetNextItemWidth(90);
    if (ImGui::Combo("##language", &language, languageOptions, 2)) localization.setLanguage(language == 1 ? ui::Language::Chinese : ui::Language::English);
    ImGui::EndMainMenuBar();
}
}

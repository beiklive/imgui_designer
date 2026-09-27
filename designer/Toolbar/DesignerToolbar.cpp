#include "DesignerToolbar.h"
#include <imgui.h>

namespace designer {
void DesignerToolbar::draw(float& zoom, std::string& path, bool& load, bool& save, ui::Localization& localization) {
    const auto file = localization.tr("menu.file");
    const auto loadLabel = localization.tr("toolbar.load");
    const auto saveLabel = localization.tr("toolbar.save");
    const auto fitLabel = localization.tr("toolbar.fit");
    const auto languageLabel = localization.tr("toolbar.language");
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu(file.c_str())) {
        if (ImGui::MenuItem(localization.tr("menu.load").c_str())) load=true;
        if (ImGui::MenuItem(localization.tr("menu.save").c_str(), "Ctrl+S")) save=true;
        ImGui::EndMenu();
    }
    ImGui::TextUnformatted(localization.tr("app.title").c_str());
    ImGui::SameLine();
    static char pathBuffer[512]{};
    if (pathBuffer[0] == '\0') std::snprintf(pathBuffer,sizeof(pathBuffer),"%s",path.c_str());
    ImGui::SetNextItemWidth(340);
    if (ImGui::InputText("##documentPath",pathBuffer,sizeof(pathBuffer))) path=pathBuffer;
    if (ImGui::Button(loadLabel.c_str())) load=true; ImGui::SameLine();
    if (ImGui::Button(saveLabel.c_str())) save=true; ImGui::SameLine();
    if (ImGui::Button("-")) zoom=std::max(.1f,zoom-.1f); ImGui::SameLine();
    if (ImGui::Button(fitLabel.c_str())) zoom=-1.0f; ImGui::SameLine();
    if (ImGui::Button("+")) zoom=std::min(3.0f,zoom+.1f); ImGui::SameLine();
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

#include "InspectorPanel.h"
#include <imgui.h>

namespace designer {
static bool colorEdit(const char* label, ui::UIColor& c) { float values[4]{c.r,c.g,c.b,c.a}; if (!ImGui::ColorEdit4(label, values)) return false; c = {values[0],values[1],values[2],values[3]}; return true; }
void InspectorPanel::draw(ui::UIElement* e, ui::Localization& localization) {
    const auto title = localization.tr("panel.inspector");
    ImGui::Begin(title.c_str());
    if (!e) { ImGui::TextDisabled(localization.tr("inspector.select_element").c_str()); ImGui::End(); return; }
    char id[256]{}; std::snprintf(id, sizeof(id), "%s", e->id.c_str());
    const auto idLabel=localization.tr("inspector.id"), typeLabel=localization.tr("inspector.type");
    if (ImGui::InputText(idLabel.c_str(), id, sizeof(id))) e->id = id;
    ImGui::Text("%s: %s", typeLabel.c_str(), e->type.c_str());
    ImGui::SeparatorText(localization.tr("inspector.layout").c_str());
    const std::string modeLabels[]{localization.tr("layout.absolute"), localization.tr("layout.horizontal"), localization.tr("layout.vertical"), localization.tr("layout.overlay")};
    const char* modes[]{modeLabels[0].c_str(), modeLabels[1].c_str(), modeLabels[2].c_str(), modeLabels[3].c_str()};
    int mode = static_cast<int>(e->layout.mode); if (ImGui::Combo((localization.tr("inspector.mode")+"##mode").c_str(), &mode, modes, 4)) e->layout.mode = static_cast<ui::LayoutMode>(mode);
    if (ImGui::DragFloat((localization.tr("inspector.x")+"##x").c_str(), &e->layout.x, 1)) {}
    if (ImGui::DragFloat((localization.tr("inspector.y")+"##y").c_str(), &e->layout.y, 1)) {}
    ImGui::DragFloat((localization.tr("inspector.width")+"##width").c_str(), &e->layout.width, 1, 0, 4096); ImGui::DragFloat((localization.tr("inspector.height")+"##height").c_str(), &e->layout.height, 1, 0, 4096);
    ImGui::DragFloat((localization.tr("inspector.spacing")+"##spacing").c_str(), &e->layout.spacing, 1, 0, 256);
    ImGui::SeparatorText(localization.tr("inspector.style").c_str());
    colorEdit((localization.tr("inspector.background")+"##background").c_str(), e->style.background); colorEdit((localization.tr("inspector.border")+"##border").c_str(), e->style.borderColor);
    ImGui::DragFloat((localization.tr("inspector.border_width")+"##border_width").c_str(), &e->style.borderWidth, .25f, 0, 32);
    ImGui::DragFloat((localization.tr("inspector.radius")+"##radius").c_str(), &e->style.radius, .25f, 0, 128);
    ImGui::DragFloat((localization.tr("inspector.shadow")+"##shadow").c_str(), &e->style.shadow, .25f, 0, 64);
    ImGui::DragFloat((localization.tr("inspector.opacity")+"##opacity").c_str(), &e->style.opacity, .01f, 0, 1);
    ImGui::Checkbox((localization.tr("inspector.clip")+"##clip").c_str(), &e->style.clip);
    ImGui::DragFloat((localization.tr("inspector.padding_left")+"##padding_left").c_str(), &e->style.paddingLeft, 1, 0, 256);
    ImGui::DragFloat((localization.tr("inspector.padding_right")+"##padding_right").c_str(), &e->style.paddingRight, 1, 0, 256);
    ImGui::DragFloat((localization.tr("inspector.padding_top")+"##padding_top").c_str(), &e->style.paddingTop, 1, 0, 256);
    ImGui::DragFloat((localization.tr("inspector.padding_bottom")+"##padding_bottom").c_str(), &e->style.paddingBottom, 1, 0, 256);
    if (e->type == "Text") {
        ImGui::SeparatorText(localization.tr("inspector.text").c_str()); char value[1024]{}; std::snprintf(value, sizeof(value), "%s", e->text.c_str());
        if (ImGui::InputTextMultiline(localization.tr("inspector.content").c_str(), value, sizeof(value), ImVec2(-1, 70))) e->text = value;
        ImGui::DragFloat((localization.tr("inspector.font_size")+"##font_size").c_str(), &e->fontSize, .5f, 6, 256); colorEdit((localization.tr("inspector.text_color")+"##text_color").c_str(), e->style.textColor);
        const char* horizontal[]{"left","center","right"}; int h=e->horizontalAlignment=="center"?1:e->horizontalAlignment=="right"?2:0;
        if (ImGui::Combo((localization.tr("inspector.horizontal")+"##horizontal").c_str(),&h,horizontal,3)) e->horizontalAlignment=horizontal[h];
        const char* vertical[]{"top","center","bottom"}; int v=e->verticalAlignment=="center"?1:e->verticalAlignment=="bottom"?2:0;
        if (ImGui::Combo((localization.tr("inspector.vertical")+"##vertical").c_str(),&v,vertical,3)) e->verticalAlignment=vertical[v];
    } else if (e->type == "Image") {
        ImGui::SeparatorText(localization.tr("inspector.image").c_str()); char value[512]{}; std::snprintf(value, sizeof(value), "%s", e->source.c_str());
        if (ImGui::InputText(localization.tr("inspector.source").c_str(), value, sizeof(value))) e->source = value;
        const std::string fitLabels[]{localization.tr("fit.contain"), localization.tr("fit.cover"), localization.tr("fit.stretch")};
        const char* fits[]{fitLabels[0].c_str(), fitLabels[1].c_str(), fitLabels[2].c_str()}; int fit = e->fit == "cover" ? 1 : e->fit == "stretch" ? 2 : 0;
        if (ImGui::Combo(localization.tr("inspector.fit").c_str(), &fit, fits, 3)) e->fit = fit == 1 ? "cover" : fit == 2 ? "stretch" : "contain";
    }
    ImGui::End();
}
}

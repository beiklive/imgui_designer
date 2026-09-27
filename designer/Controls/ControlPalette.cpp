#include "ControlPalette.h"
#include "../../ui/widgets/Box.h"
#include "../../ui/widgets/Image.h"
#include "../../ui/widgets/Label.h"
#include "../../ui/widgets/Text.h"
#include <imgui.h>
#include <memory>

namespace designer {
ui::UIElement* ControlPalette::addElement(ui::UIElement& root, const std::string& type) {
    std::unique_ptr<ui::UIElement> element;
    if (type == "Box") element = std::make_unique<ui::Box>();
    else if (type == "Image") element = std::make_unique<ui::Image>();
    else if (type == "Label") element = std::make_unique<ui::Label>();
    else element = std::make_unique<ui::Text>();
    element->id = type + "_new_" + std::to_string(idCounter_++);
    element->layout.width = 240;
    element->layout.height = type == "Box" ? 80 : 40;
    if (type == "Image") element->source = "image.jpg";
    if (type == "Label") element->text = "Label";
    if (type == "Text") element->text = "Text";
    return &root.addChild(std::move(element));
}

void ControlPalette::draw(ui::UIElement*& selection, std::string& importPath, bool& newCanvas, bool& importComponent, ui::Localization& localization) {
    ImGui::Begin(localization.tr("panel.controls").c_str());
    if (ImGui::Button(localization.tr("controls.new_canvas").c_str())) newCanvas = true;
    ImGui::SeparatorText(localization.tr("controls.primitives").c_str());
    ui::UIElement* target = selection;
    if (target && (target->type == "Text" || target->type == "Label" || target->type == "Image")) target = target->parent();
    if (target && ImGui::Button(localization.tr("controls.add_box").c_str())) selection = addElement(*target, "Box");
    if (target && ImGui::Button(localization.tr("controls.add_text").c_str())) selection = addElement(*target, "Text");
    if (target && ImGui::Button(localization.tr("controls.add_label").c_str())) selection = addElement(*target, "Label");
    if (target && ImGui::Button(localization.tr("controls.add_image").c_str())) selection = addElement(*target, "Image");
    ImGui::SeparatorText(localization.tr("controls.import").c_str());
    static char path[512]{};
    static std::string lastPath;
    if (lastPath != importPath) { std::snprintf(path, sizeof(path), "%s", importPath.c_str()); lastPath = importPath; }
    if (ImGui::InputText("##importPath", path, sizeof(path))) importPath = path;
    if (ImGui::Button(localization.tr("controls.import_button").c_str())) importComponent = true;
    ImGui::End();
}
}

#include "HierarchyPanel.h"
#include <imgui.h>
#include <cstdint>

namespace designer {
void HierarchyPanel::draw(ui::UIElement& root, ui::UIElement*& selection, bool& deleteRequested, ui::Localization& localization) {
    const auto title = localization.tr("panel.hierarchy");
    ImGui::Begin(title.c_str()); drawElement(root, selection, deleteRequested, localization); ImGui::End();
}
void HierarchyPanel::drawElement(ui::UIElement& e, ui::UIElement*& selection, bool& deleteRequested, ui::Localization& localization) {
    const bool leaf = e.children().empty();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
    if (!e.parent()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
    if (leaf) flags |= ImGuiTreeNodeFlags_Leaf;
    if (selection == &e) flags |= ImGuiTreeNodeFlags_Selected;
    const std::string label = (e.id.empty() ? e.type : e.id + "  [" + e.type + "]") + "##" + std::to_string(reinterpret_cast<std::uintptr_t>(&e));
    const bool open = ImGui::TreeNodeEx(label.c_str(), flags);
    if (ImGui::IsItemClicked()) selection = &e;
    if (ImGui::BeginPopupContextItem()) {
        selection = &e;
        if (e.parent() && ImGui::MenuItem(localization.tr("controls.delete").c_str())) deleteRequested = true;
        ImGui::EndPopup();
    }
    if (open) { for (const auto& child : e.children()) drawElement(*child, selection, deleteRequested, localization); ImGui::TreePop(); }
}
}

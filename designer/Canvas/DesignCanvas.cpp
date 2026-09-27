#include "DesignCanvas.h"
#include <imgui.h>
#include <algorithm>

namespace designer {
ui::UIElement* DesignCanvas::hitTest(ui::UIElement& e, float x, float y) const {
    const auto r = e.computedRect;
    if (x < r.x || y < r.y || x > r.x+r.width || y > r.y+r.height) return nullptr;
    for (auto it=e.children().rbegin(); it!=e.children().rend(); ++it) if (auto* hit=hitTest(**it,x,y)) return hit;
    return &e;
}
void DesignCanvas::draw(ui::UIRuntime& runtime, ui::ImGuiUIRenderer& renderer, ui::UIElement*& selected, float& zoom, ui::Localization& localization) {
    const auto title = localization.tr("panel.canvas");
    ImGui::Begin(title.c_str());
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const ImVec2 childOrigin = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("CanvasSurface", avail, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);
    if (zoom <= 0) { zoom=std::clamp(std::min(avail.x/1280.0f,avail.y/720.0f)*.92f,.1f,3.0f); panX_=panY_=0; }
    const ImVec2 origin{childOrigin.x + (avail.x - 1280*zoom)*.5f + panX_, childOrigin.y + (avail.y - 720*zoom)*.5f + panY_};
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(childOrigin, {childOrigin.x+avail.x, childOrigin.y+avail.y}, IM_COL32(30,32,37,255));
    const ImVec2 canvasMax{origin.x+1280*zoom, origin.y+720*zoom};
    dl->AddRectFilled(origin, canvasMax, IM_COL32(248,248,248,255));
    runtime.layout({0,0,1280,720});
    runtime.render(renderer, {origin.x,origin.y,1280,720}, zoom);

    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const bool inside = mouse.x>=origin.x && mouse.y>=origin.y && mouse.x<=canvasMax.x && mouse.y<=canvasMax.y;
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) { panning_=true; panMouse_=mouse; }
    if (panning_) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) { panX_+=mouse.x-panMouse_.x; panY_+=mouse.y-panMouse_.y; panMouse_=mouse; }
        else panning_=false;
    }
    if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel != 0 && ImGui::GetIO().KeyCtrl) zoom=std::clamp(zoom+ImGui::GetIO().MouseWheel*.05f,.1f,3.0f);
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && inside && runtime.document().root) {
        const float x=(mouse.x-origin.x)/zoom, y=(mouse.y-origin.y)/zoom;
        selected=hitTest(*runtime.document().root,x,y);
        dragging_=selected && selected->parent() && selected->parent()->layout.mode==ui::LayoutMode::Absolute;
        resizing_=dragging_ && x>selected->computedRect.x+selected->computedRect.width-12/zoom && y>selected->computedRect.y+selected->computedRect.height-12/zoom;
        dragMouse_=mouse;
        if (dragging_) { dragX_=selected->layout.x; dragY_=selected->layout.y; dragW_=selected->layout.width; dragH_=selected->layout.height; }
    }
    if (dragging_ && ImGui::IsMouseDown(ImGuiMouseButton_Left) && selected) {
        const float dx=(mouse.x-dragMouse_.x)/zoom, dy=(mouse.y-dragMouse_.y)/zoom;
        if (resizing_) { selected->layout.width=std::max(1.0f,dragW_+dx); selected->layout.height=std::max(1.0f,dragH_+dy); }
        else { selected->layout.x=dragX_+dx; selected->layout.y=dragY_+dy; }
        runtime.layout({0,0,1280,720});
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) dragging_=false;
    if (selected) {
        auto r=selected->computedRect; ImVec2 a{origin.x+r.x*zoom,origin.y+r.y*zoom}, b{a.x+r.width*zoom,a.y+r.height*zoom};
        dl->AddRect(a,b,IM_COL32(55,150,255,255),0.0f,2.0f,0);
        dl->AddRectFilled({b.x-5,b.y-5},{b.x+5,b.y+5},IM_COL32(55,150,255,255));
    }
    ImGui::SetCursorScreenPos({childOrigin.x,childOrigin.y+avail.y-22});
    const auto noSelection = localization.tr("canvas.no_selection");
    ImGui::Text("1280 x 720    %.0f%%    %s",zoom*100,selected?selected->id.c_str():noSelection.c_str());
    ImGui::End();
}
}

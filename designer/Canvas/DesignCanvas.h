#pragma once
#include "../../ui/runtime/UIRuntime.h"
#include "../../ui/renderer/imgui/ImGuiUIRenderer.h"
#include "../../ui/i18n/Localization.h"
#include <imgui.h>
namespace designer { class DesignCanvas { public: void draw(ui::UIRuntime& runtime, ui::ImGuiUIRenderer& renderer, ui::UIElement*& selection, float& zoom, bool running, bool& deleteRequested, ui::Localization& localization); private: ui::UIElement* hitTest(ui::UIElement& e, float x, float y) const; bool dragging_ = false, resizing_ = false, panning_ = false; ImVec2 dragMouse_{}, panMouse_{}; float dragX_=0, dragY_=0, dragW_=0, dragH_=0, panX_=0, panY_=0; };
}

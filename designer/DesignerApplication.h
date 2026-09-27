#pragma once
#include "../ui/runtime/UIRuntime.h"
#include "../ui/i18n/Localization.h"
#include "../ui/renderer/imgui/ImGuiUIRenderer.h"
#include "../ui/theme/UITheme.h"
#include "UICommand.h"
#include <string>

struct GLFWwindow;
namespace designer {
class DesignerApplication {
public:
    bool initialize(const std::string& initialDocument);
    int run();
    void shutdown();
private:
    void drawMainMenu();
    void drawStatusBar();
    void loadFonts();
    void select(ui::UIElement* element) { selection_ = element; }
    ui::UIRuntime runtime_;
    ui::ImGuiUIRenderer renderer_;
    ui::UITheme theme_;
    ui::Localization localization_;
    ui::UIElement* selection_ = nullptr;
    GLFWwindow* window_ = nullptr;
    std::string documentPath_;
    std::string error_;
    float zoom_ = -1.0f;
    CommandHistory history_;
};
}

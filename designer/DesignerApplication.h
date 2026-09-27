#pragma once
#include "../ui/runtime/UIRuntime.h"
#include "../ui/i18n/Localization.h"
#include "../ui/renderer/imgui/ImGuiUIRenderer.h"
#include "../ui/theme/UITheme.h"
#include "UICommand.h"
#include "DesignerSettings.h"
#include <filesystem>
#include <vector>
#include <string>

struct GLFWwindow;
namespace designer {
struct DesignDocument {
    std::string path;
    ui::UIRuntime runtime;
    ui::UIElement* selection = nullptr;
    std::string importPath;
    float zoom = -1.0f;
    bool running = false;
    bool dirty = false;
};

class DesignerApplication {
public:
    bool initialize(const std::string& initialDocument);
    int run();
    void shutdown();
private:
    void drawMainMenu();
    void drawStatusBar();
    void loadFonts();
    void drawDocumentTabs();
    void deleteSelection(bool& deleteRequested);
    bool loadIntoDocument(DesignDocument& document, const std::string& path);
    bool saveDocument(DesignDocument& document);
    DesignDocument& activeDocument() { return documents_.at(activeIndex_); }
    const DesignDocument& activeDocument() const { return documents_.at(activeIndex_); }
    void createDocument();
    void importComponent(DesignDocument& document);

    ui::ImGuiUIRenderer renderer_;
    ui::UITheme theme_;
    ui::Localization localization_;
    DesignerSettings settings_;
    std::filesystem::path configPath_;
    std::vector<DesignDocument> documents_;
    size_t activeIndex_ = 0;
    GLFWwindow* window_ = nullptr;
    std::string error_;
    CommandHistory history_;
};
}

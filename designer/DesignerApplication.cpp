#include "DesignerApplication.h"
#include "Canvas/DesignCanvas.h"
#include "Controls/ControlPalette.h"
#include "Hierarchy/HierarchyPanel.h"
#include "Inspector/InspectorPanel.h"
#include "Toolbar/DesignerToolbar.h"
#include "../ui/parser/UISerializer.h"
#include "../ui/widgets/Box.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <exception>
#include <cstddef>
#include <filesystem>

namespace designer {
namespace {
ui::UIDocument newDocumentTree() {
    ui::UIDocument document;
    document.root = std::make_unique<ui::Box>();
    document.root->id = "root";
    document.root->layout.mode = ui::LayoutMode::Vertical;
    document.root->layout.width = 1280;
    document.root->layout.height = 720;
    document.root->style.background = {0.10f, 0.11f, 0.13f, 1};
    return document;
}
std::string displayName(const std::string& path, size_t index) {
    if (!path.empty()) return std::filesystem::path(path).filename().string();
    return "Untitled " + std::to_string(index + 1);
}
}

bool DesignerApplication::initialize(const std::string& initialDocument) {
    configPath_ = std::filesystem::path(UI_DESIGNER_SOURCE_DIR) / "designer.config.json";
    settings_.load(configPath_);
    localization_.loadDirectory(std::filesystem::path(UI_DESIGNER_SOURCE_DIR) / "resources" / "i18n");
    localization_.setLanguage(settings_.language);
    if (!glfwInit()) { error_=localization_.tr("error.glfw_init"); return false; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    window_=glfwCreateWindow(settings_.windowWidth, settings_.windowHeight, localization_.tr("app.title").c_str(), nullptr, nullptr);
    if (!window_) { error_=localization_.tr("error.window"); glfwTerminate(); return false; }
    glfwMakeContextCurrent(window_); glfwSwapInterval(1);
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    ImGui::StyleColorsDark();
    loadFonts();
    ImGui_ImplGlfw_InitForOpenGL(window_,true); ImGui_ImplOpenGL3_Init("#version 150");

    const std::string startPath = initialDocument.empty() ? settings_.lastDocument : initialDocument;
    DesignDocument document;
    if (!startPath.empty() && loadIntoDocument(document, startPath)) documents_.push_back(std::move(document));
    else createDocument();
    return true;
}

bool DesignerApplication::loadIntoDocument(DesignDocument& document, const std::string& path) {
    try {
        document.runtime.load(path);
        document.path = path;
        document.selection = document.runtime.document().root.get();
        document.zoom = -1.0f;
        document.dirty = false;
        renderer_.setAssetBaseDirectory(std::filesystem::path(path).parent_path());
        return true;
    } catch (const std::exception& ex) {
        error_ = localization_.tr("error.load") + ": " + ex.what();
        return false;
    }
}

bool DesignerApplication::saveDocument(DesignDocument& document) {
    if (document.path.empty()) {
        error_ = localization_.tr("error.save") + ": no document path";
        return false;
    }
    try {
        if (document.runtime.document().root) ui::UISerializer{}.save(*document.runtime.document().root, document.path, document.runtime.document().version);
        document.dirty = false;
        error_.clear();
        return true;
    } catch (const std::exception& ex) {
        error_ = localization_.tr("error.save") + ": " + ex.what();
        return false;
    }
}

void DesignerApplication::createDocument() {
    DesignDocument document;
    document.runtime.setDocument(newDocumentTree());
    document.selection = document.runtime.document().root.get();
    document.path = {};
    document.zoom = -1.0f;
    documents_.push_back(std::move(document));
    activeIndex_ = documents_.empty() ? 0 : documents_.size() - 1;
}

void DesignerApplication::importComponent(DesignDocument& document) {
    if (document.importPath.empty() || !document.runtime.document().root) return;
    try {
        ui::UIDocument imported = ui::UIParser{}.load(document.importPath);
        if (!imported.root) return;
        ui::UIElement* parent = document.selection ? document.selection : document.runtime.document().root.get();
        if (parent->type == "Text" || parent->type == "Label" || parent->type == "Image") parent = parent->parent() ? parent->parent() : document.runtime.document().root.get();
        document.selection = &parent->addChild(std::move(imported.root));
        document.dirty = true;
        error_.clear();
    } catch (const std::exception& ex) {
        error_ = localization_.tr("error.import") + ": " + ex.what();
    }
}

void DesignerApplication::deleteSelection(bool& deleteRequested) {
    if (!deleteRequested) return;
    deleteRequested = false;
    auto& document = activeDocument();
    if (!document.selection || !document.selection->parent()) {
        error_ = localization_.tr("error.delete_root");
        return;
    }
    ui::UIElement* parent = document.selection->parent();
    parent->removeChild(document.selection);
    document.selection = parent;
    document.dirty = true;
    error_.clear();
}

void DesignerApplication::drawDocumentTabs() {
    int width=0, height=0;
    glfwGetWindowSize(window_, &width, &height);
    ImGui::SetNextWindowPos({0, 22});
    ImGui::SetNextWindowSize({static_cast<float>(width), 30});
    if (!ImGui::Begin("##DocumentTabs", nullptr, ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize)) return;
    if (ImGui::Button(localization_.tr("tabs.new").c_str())) createDocument();
    ImGui::SameLine();
    for (size_t index = 0; index < documents_.size(); ++index) {
        ImGui::PushID(static_cast<int>(index));
        const bool selected = index == activeIndex_;
        const std::string label = displayName(documents_[index].path, index) + (documents_[index].dirty ? " *" : "");
        if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick, {150, 0})) {
            activeIndex_ = index;
            renderer_.setAssetBaseDirectory(std::filesystem::path(activeDocument().path).parent_path());
        }
        if (ImGui::BeginPopupContextItem("TabContext")) {
            if (ImGui::MenuItem(localization_.tr("tabs.close").c_str()) && documents_.size() > 1) {
                documents_.erase(documents_.begin() + static_cast<std::ptrdiff_t>(index));
                if (activeIndex_ >= documents_.size()) activeIndex_ = documents_.size() - 1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();
        if (index + 1 < documents_.size()) ImGui::SameLine();
    }
    ImGui::End();
}

int DesignerApplication::run() {
    HierarchyPanel hierarchy; InspectorPanel inspector; DesignCanvas canvas; DesignerToolbar toolbar; ControlPalette controls;
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents(); ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
        bool load=false, save=false, newCanvas=false, importComponentRequested=false, deleteRequested=false;
        const size_t toolbarIndex = activeIndex_;
        toolbar.draw(documents_[toolbarIndex].zoom, documents_[toolbarIndex].path, load, save, documents_[toolbarIndex].running, localization_);
        drawDocumentTabs();
        auto& document = activeDocument();
        glfwSetWindowTitle(window_, localization_.tr("app.title").c_str());
        if (load) {
            DesignDocument loaded;
            if (loadIntoDocument(loaded, document.path)) document = std::move(loaded);
        }
        if (save) saveDocument(document);
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) saveDocument(document);
        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) deleteRequested = true;
        if (document.running) document.runtime.update(ImGui::GetIO().DeltaTime);

        int windowWidth=0, windowHeight=0; glfwGetWindowSize(window_,&windowWidth,&windowHeight);
        const float side=240.0f, status=30.0f, menu=52.0f, controlsHeight=205.0f;
        ImGui::SetNextWindowPos({0,menu},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({side,controlsHeight},ImGuiCond_FirstUseEver);
        controls.draw(document.selection, document.importPath, newCanvas, importComponentRequested, localization_);
        ImGui::SetNextWindowPos({0,menu+controlsHeight},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({side,windowHeight-menu-controlsHeight-status},ImGuiCond_FirstUseEver);
        if (document.runtime.document().root) hierarchy.draw(*document.runtime.document().root,document.selection,deleteRequested,localization_);
        ImGui::SetNextWindowPos({static_cast<float>(windowWidth)-side,menu},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({side,windowHeight-menu-status},ImGuiCond_FirstUseEver);
        inspector.draw(document.selection,localization_);
        ImGui::SetNextWindowPos({side,menu},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({static_cast<float>(windowWidth)-side*2,windowHeight-menu-status},ImGuiCond_FirstUseEver);
        canvas.draw(document.runtime,renderer_,document.selection,document.zoom,document.running,deleteRequested,localization_);
        if (importComponentRequested) importComponent(document);
        deleteSelection(deleteRequested);
        drawStatusBar();
        ImGui::Render(); int w,h; glfwGetFramebufferSize(window_,&w,&h); glViewport(0,0,w,h);
        glClearColor(.08f,.085f,.10f,1); glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); glfwSwapBuffers(window_);
        if (newCanvas) createDocument();
    }
    return 0;
}

void DesignerApplication::drawStatusBar() {
    int width=0,height=0; glfwGetWindowSize(window_,&width,&height);
    ImGui::SetNextWindowPos({0,static_cast<float>(height-26)}); ImGui::SetNextWindowSize({static_cast<float>(width),26});
    if (ImGui::Begin("##StatusBar",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoInputs)) {
        const auto documentLabel = localization_.tr("status.document");
        const auto mode = activeDocument().running ? localization_.tr("tabs.running") : localization_.tr("tabs.design");
        ImGui::Text("%s: %s  |  %s",documentLabel.c_str(),activeDocument().path.empty()?"Untitled":activeDocument().path.c_str(),mode.c_str()); ImGui::SameLine();
        if (!error_.empty()) ImGui::TextColored({1,.35f,.3f,1},"%s",error_.c_str());
        else { const auto ready=localization_.tr("status.ready"), tree=localization_.tr("status.shared_tree"), core=localization_.tr("status.core_independent"); ImGui::Text("%s   |   %s   |   %s",ready.c_str(),tree.c_str(),core.c_str()); }
        ImGui::End();
    }
}

void DesignerApplication::loadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    const ImWchar* ranges = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();
    const std::filesystem::path candidates[] = {
        std::filesystem::path(UI_DESIGNER_SOURCE_DIR) / "resources" / "fonts" / "NotoSansCJK-Regular.ttc",
#ifdef _WIN32
        "C:/Windows/Fonts/msyh.ttc", "C:/Windows/Fonts/simhei.ttf",
#elif defined(__APPLE__)
        "/System/Library/Fonts/PingFang.ttc",
#else
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc", "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttf",
#endif
    };
    for (const auto& path : candidates) {
        if (!std::filesystem::exists(path)) continue;
        if (io.Fonts->AddFontFromFileTTF(path.string().c_str(), 18.0f, nullptr, ranges)) { io.FontDefault = io.Fonts->Fonts.back(); return; }
    }
}

void DesignerApplication::shutdown() {
    if (window_) {
        glfwGetWindowSize(window_, &settings_.windowWidth, &settings_.windowHeight);
        settings_.language = localization_.language();
        settings_.lastDocument = documents_.empty() ? std::string{} : activeDocument().path;
        settings_.save(configPath_);
    }
    renderer_.releaseTextures();
    if (ImGui::GetCurrentContext()) { ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext(); }
    if (window_) { glfwDestroyWindow(window_); window_=nullptr; }
    glfwTerminate();
}
}

#include "DesignerApplication.h"
#include "Canvas/DesignCanvas.h"
#include "Hierarchy/HierarchyPanel.h"
#include "Inspector/InspectorPanel.h"
#include "Toolbar/DesignerToolbar.h"
#include "../ui/parser/UISerializer.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <exception>
#include <filesystem>

namespace designer {
bool DesignerApplication::initialize(const std::string& initialDocument) {
    localization_.loadDirectory(std::filesystem::path(UI_DESIGNER_SOURCE_DIR) / "resources" / "i18n");
    if (!glfwInit()) { error_=localization_.tr("error.glfw_init"); return false; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    window_=glfwCreateWindow(1440,900,localization_.tr("app.title").c_str(),nullptr,nullptr);
    if (!window_) { error_=localization_.tr("error.window"); glfwTerminate(); return false; }
    glfwMakeContextCurrent(window_); glfwSwapInterval(1);
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    ImGui::StyleColorsDark();
    loadFonts();
    ImGui_ImplGlfw_InitForOpenGL(window_,true); ImGui_ImplOpenGL3_Init("#version 150");
    documentPath_=initialDocument;
    renderer_.setAssetBaseDirectory(std::filesystem::path(documentPath_).parent_path());
    try { runtime_.load(documentPath_); selection_=runtime_.document().root.get(); }
    catch (const std::exception& ex) { error_=localization_.tr("error.load") + ": " + ex.what(); return false; }
    return true;
}
int DesignerApplication::run() {
    HierarchyPanel hierarchy; InspectorPanel inspector; DesignCanvas canvas; DesignerToolbar toolbar;
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents(); ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
        bool load=false, save=false;
        toolbar.draw(zoom_,documentPath_,load,save,localization_);
        glfwSetWindowTitle(window_, localization_.tr("app.title").c_str());
        if (load) { try { runtime_.load(documentPath_); renderer_.setAssetBaseDirectory(std::filesystem::path(documentPath_).parent_path()); selection_=runtime_.document().root.get(); error_.clear(); } catch (const std::exception& ex) { error_=localization_.tr("error.load") + ": " + ex.what(); } }
        if (save) { try { if (runtime_.document().root) ui::UISerializer{}.save(*runtime_.document().root,documentPath_,runtime_.document().version); error_.clear(); } catch (const std::exception& ex) { error_=localization_.tr("error.save") + ": " + ex.what(); } }
        if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
            try { if (runtime_.document().root) ui::UISerializer{}.save(*runtime_.document().root,documentPath_,runtime_.document().version); } catch (const std::exception& ex) { error_=localization_.tr("error.save") + ": " + ex.what(); }
        }
        int windowWidth=0, windowHeight=0; glfwGetWindowSize(window_,&windowWidth,&windowHeight);
        runtime_.update(ImGui::GetIO().DeltaTime);
        const float side=240.0f, status=30.0f, menu=22.0f;
        ImGui::SetNextWindowPos({0,menu},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({side,windowHeight-menu-status},ImGuiCond_FirstUseEver);
        if (runtime_.document().root) hierarchy.draw(*runtime_.document().root,selection_,localization_);
        ImGui::SetNextWindowPos({static_cast<float>(windowWidth)-side,menu},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({side,windowHeight-menu-status},ImGuiCond_FirstUseEver);
        inspector.draw(selection_,localization_);
        ImGui::SetNextWindowPos({side,menu},ImGuiCond_FirstUseEver); ImGui::SetNextWindowSize({static_cast<float>(windowWidth)-side*2,windowHeight-menu-status},ImGuiCond_FirstUseEver);
        canvas.draw(runtime_,renderer_,selection_,zoom_,localization_);
        drawStatusBar();
        ImGui::Render(); int w,h; glfwGetFramebufferSize(window_,&w,&h); glViewport(0,0,w,h);
        glClearColor(.08f,.085f,.10f,1); glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); glfwSwapBuffers(window_);
    }
    return 0;
}
void DesignerApplication::drawStatusBar() {
    int width=0,height=0; glfwGetWindowSize(window_,&width,&height);
    ImGui::SetNextWindowPos({0,static_cast<float>(height-26)});
    ImGui::SetNextWindowSize({static_cast<float>(width),26});
    if (ImGui::Begin("##StatusBar",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoInputs)) {
        const auto document = localization_.tr("status.document");
        ImGui::Text("%s: %s",document.c_str(),documentPath_.c_str()); ImGui::SameLine();
        if (!error_.empty()) { ImGui::TextColored({1,.35f,.3f,1},"%s",error_.c_str()); }
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
    renderer_.releaseTextures();
    if (ImGui::GetCurrentContext()) { ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext(); }
    if (window_) { glfwDestroyWindow(window_); window_=nullptr; }
    glfwTerminate();
}
}

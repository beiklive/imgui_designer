#pragma once
#include "../UIRenderer.h"
#include <filesystem>
#include <memory>

namespace ui { class ImageTextureCache; }

namespace ui {
class ImGuiUIRenderer final : public UIRenderer, private UIRenderContext {
public:
    ImGuiUIRenderer();
    ~ImGuiUIRenderer() override;
    ImGuiUIRenderer(const ImGuiUIRenderer&) = delete;
    ImGuiUIRenderer& operator=(const ImGuiUIRenderer&) = delete;

    void setAssetBaseDirectory(std::filesystem::path directory);
    void releaseTextures();
    void beginFrame(UIRect viewport, float scale = 1.0f) override;
    void render(UIElement& root) override;
    void endFrame() override;
private:
    void renderElement(UIElement& element);
    void box(const UIRect&, const UIColor&, const UIColor&, float, float, float) override;
    void text(const UIRect&, const char*, float, const UIColor&, const char* = "left", const char* = "top") override;
    void image(const UIRect&, const char*, const char*, float, const char*, const char*) override;
    void pushClip(const UIRect&) override;
    void popClip() override;
    UIRect viewport_{};
    float scale_ = 1;
    std::filesystem::path assetBaseDirectory_;
    std::unique_ptr<ImageTextureCache> textures_;
};
}

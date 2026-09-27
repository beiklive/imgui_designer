#include "ImGuiUIRenderer.h"
#include "ImageTextureCache.h"
#include <imgui.h>
#include <algorithm>
#include <cstring>
#include <filesystem>

namespace ui {
static ImU32 packed(UIColor c) { return ImGui::ColorConvertFloat4ToU32({c.r, c.g, c.b, c.a}); }
ImGuiUIRenderer::ImGuiUIRenderer() : textures_(std::make_unique<ImageTextureCache>()) {}
ImGuiUIRenderer::~ImGuiUIRenderer() { releaseTextures(); }
void ImGuiUIRenderer::setAssetBaseDirectory(std::filesystem::path directory) { assetBaseDirectory_ = std::move(directory); }
void ImGuiUIRenderer::releaseTextures() { if (textures_) textures_->release(); }
void ImGuiUIRenderer::beginFrame(UIRect viewport, float scale) { viewport_ = viewport; scale_ = scale; }
void ImGuiUIRenderer::render(UIElement& root) { renderElement(root); }
void ImGuiUIRenderer::endFrame() {}
void ImGuiUIRenderer::renderElement(UIElement& e) {
    const auto r = e.computedRect;
    if (e.scrollable) {
        const float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0 && ImGui::IsMouseHoveringRect({viewport_.x+r.x*scale_,viewport_.y+r.y*scale_},{viewport_.x+(r.x+r.width)*scale_,viewport_.y+(r.y+r.height)*scale_})) {
            if (e.horizontalScroll) e.scrollX = std::clamp(e.scrollX - wheel * 40.0f, 0.0f, std::max(0.0f, e.contentRect.width-r.width));
            else e.scrollY = std::clamp(e.scrollY - wheel * 40.0f, 0.0f, std::max(0.0f, e.contentRect.height-r.height));
        }
    }
    if (e.scrollable || e.style.clip) pushClip(r);
    if (e.type == "Box" || e.scrollable) {
        auto bg=e.style.background, border=e.style.borderColor;
        bg.a*=e.style.opacity; border.a*=e.style.opacity;
        box(r, bg, border, e.style.radius, e.style.borderWidth, e.style.shadow * e.style.opacity);
    }
    else if (e.type == "Text" || e.type == "Label") { auto color=e.style.textColor; color.a*=e.style.opacity; text(r, e.text.c_str(), e.fontSize, color, e.horizontalAlignment.c_str(), e.verticalAlignment.c_str()); }
    else if (e.type == "Image") image(r, e.source.c_str(), e.fit.c_str(), e.style.opacity, e.horizontalAlignment.c_str(), e.verticalAlignment.c_str());
    for (const auto& child : e.children()) renderElement(*child);
    if (e.scrollable || e.style.clip) popClip();
}
void ImGuiUIRenderer::box(const UIRect& r, const UIColor& bg, const UIColor& border, float radius, float borderWidth, float shadow) {
    auto* dl = ImGui::GetWindowDrawList();
    const ImVec2 a(viewport_.x + r.x * scale_, viewport_.y + r.y * scale_);
    const ImVec2 b(a.x + r.width * scale_, a.y + r.height * scale_);
    for (int i=3; i>=1 && shadow>0; --i) {
        UIColor shade{0,0,0,shadow * 0.08f};
        const float spread=static_cast<float>(i)*scale_;
        dl->AddRectFilled({a.x-spread,a.y-spread+shadow*.15f},{b.x+spread,b.y+spread+shadow*.15f},packed(shade),(radius+i)*scale_);
    }
    dl->AddRectFilled(a, b, packed(bg), radius * scale_);
    if (borderWidth > 0) dl->AddRect(a,b,packed(border),radius*scale_,borderWidth*scale_,ImDrawFlags_None);
}
void ImGuiUIRenderer::text(const UIRect& r, const char* value, float size, const UIColor& color, const char* horizontal, const char* vertical) {
    auto* dl = ImGui::GetWindowDrawList();
    const ImVec2 measured=ImGui::CalcTextSize(value);
    float x=r.x, y=r.y;
    if (horizontal && std::strcmp(horizontal,"center")==0) x+=(r.width-measured.x/scale_)*.5f;
    else if (horizontal && std::strcmp(horizontal,"right")==0) x+=r.width-measured.x/scale_;
    if (vertical && std::strcmp(vertical,"center")==0) y+=(r.height-measured.y/scale_)*.5f;
    else if (vertical && std::strcmp(vertical,"bottom")==0) y+=r.height-measured.y/scale_;
    dl->AddText(nullptr, size * scale_, {viewport_.x + x * scale_, viewport_.y + y * scale_}, packed(color), value);
}
void ImGuiUIRenderer::image(const UIRect& r, const char* source, const char* fit, float opacity, const char* horizontal, const char* vertical) {
    std::filesystem::path path = source ? std::filesystem::path(source) : std::filesystem::path{};
    if (!path.empty() && path.is_relative()) path = assetBaseDirectory_ / path;
    const ImageTexture* texture = textures_ ? textures_->load(path) : nullptr;
    if (!texture) {
        box(r, {0.18f, 0.20f, 0.24f, opacity}, {0.35f, 0.39f, 0.45f, opacity}, 2, 1, 0);
        text(r, source && *source ? source : "Image", 12, {0.75f, 0.78f, 0.83f, opacity}, horizontal, vertical);
        return;
    }

    const float sourceAspect = static_cast<float>(texture->width) / static_cast<float>(texture->height);
    const float destinationAspect = r.width / std::max(1.0f, r.height);
    float drawWidth = r.width, drawHeight = r.height;
    if (fit && std::strcmp(fit, "contain") == 0) {
        if (sourceAspect > destinationAspect) drawHeight = drawWidth / sourceAspect;
        else drawWidth = drawHeight * sourceAspect;
    } else if (fit && std::strcmp(fit, "cover") == 0) {
        if (sourceAspect > destinationAspect) drawWidth = drawHeight * sourceAspect;
        else drawHeight = drawWidth / sourceAspect;
    }
    float x = r.x, y = r.y;
    if (horizontal && std::strcmp(horizontal, "center") == 0) x += (r.width - drawWidth) * 0.5f;
    else if (horizontal && std::strcmp(horizontal, "right") == 0) x += r.width - drawWidth;
    if (vertical && std::strcmp(vertical, "center") == 0) y += (r.height - drawHeight) * 0.5f;
    else if (vertical && std::strcmp(vertical, "bottom") == 0) y += r.height - drawHeight;

    ImVec2 minimum{viewport_.x + x * scale_, viewport_.y + y * scale_};
    ImVec2 maximum{minimum.x + drawWidth * scale_, minimum.y + drawHeight * scale_};
    auto* drawList = ImGui::GetWindowDrawList();
    const auto tint = ImGui::ColorConvertFloat4ToU32({1, 1, 1, opacity});
    if (fit && std::strcmp(fit, "cover") == 0) {
        const float uvWidth = r.width / drawWidth;
        const float uvHeight = r.height / drawHeight;
        minimum = {viewport_.x + r.x * scale_, viewport_.y + r.y * scale_};
        maximum = {minimum.x + r.width * scale_, minimum.y + r.height * scale_};
        drawList->AddImage(ImTextureRef(static_cast<ImTextureID>(static_cast<intptr_t>(texture->id))), minimum, maximum,
            {(1.0f - uvWidth) * 0.5f, (1.0f - uvHeight) * 0.5f}, {(1.0f + uvWidth) * 0.5f, (1.0f + uvHeight) * 0.5f}, tint);
    } else {
        drawList->AddImage(ImTextureRef(static_cast<ImTextureID>(static_cast<intptr_t>(texture->id))), minimum, maximum,
            {0, 0}, {1, 1}, tint);
    }
}
void ImGuiUIRenderer::pushClip(const UIRect& r) {
    ImGui::GetWindowDrawList()->PushClipRect({viewport_.x + r.x * scale_, viewport_.y + r.y * scale_},
        {viewport_.x + (r.x + r.width) * scale_, viewport_.y + (r.y + r.height) * scale_}, true);
}
void ImGuiUIRenderer::popClip() { ImGui::GetWindowDrawList()->PopClipRect(); }
}

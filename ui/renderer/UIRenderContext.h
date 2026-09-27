#pragma once
#include "../core/UIRect.h"

namespace ui {
class UIRenderContext {
public:
    virtual ~UIRenderContext() = default;
    virtual void box(const UIRect&, const UIColor&, const UIColor&, float, float, float) = 0;
    virtual void text(const UIRect&, const char*, float, const UIColor&, const char* = "left", const char* = "top") = 0;
    virtual void image(const UIRect&, const char*, const char*, float, const char*, const char*) = 0;
    virtual void pushClip(const UIRect&) = 0;
    virtual void popClip() = 0;
};
}

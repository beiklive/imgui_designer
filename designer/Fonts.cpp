#include "Fonts.h"

#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <vector>

namespace designer {
namespace {

// 文本 / 按键图标 / Material 图标：决定这个字体源负责哪些私用区码位。
enum class Content { Text, ButtonIcons, MaterialIcons };

struct Source {
    std::filesystem::path path;
    float sizePixels = 18.0f;
    bool merge = false;
    Content content = Content::Text;
};

std::vector<std::uint32_t> ownedCodepoints(Content content) {
    std::vector<std::uint32_t> codes;
    if (content == Content::ButtonIcons) {
        codes.assign(std::begin(kButtonIconCodepoints), std::end(kButtonIconCodepoints));
    } else if (content == Content::MaterialIcons) {
        codes.assign(std::begin(kMaterialIconCodepoints), std::end(kMaterialIconCodepoints));
    }
    std::sort(codes.begin(), codes.end());
    codes.erase(std::unique(codes.begin(), codes.end()), codes.end());
    return codes;
}

// 码位列表 -> imgui 要的 [first,last] 区间数组（0 结尾）。
// imgui 限制这个数组不超过 64 项，所以中间没有本源码位的相邻区间直接连起来
// （多排掉几个私用区空码位没有副作用）。
std::vector<ImWchar> buildExcludeRanges(const std::vector<std::uint32_t>& blocked,
                                        const std::vector<std::uint32_t>& own) {
    std::vector<ImWchar> ranges;
    for (std::uint32_t code : blocked) {
        const bool extendable = !ranges.empty() && ranges.size() % 2 == 0 &&
                                code >= static_cast<std::uint32_t>(ranges.back()) + 1u &&
                                std::none_of(own.begin(), own.end(), [&](std::uint32_t c) {
                                    return c > static_cast<std::uint32_t>(ranges.back()) && c < code;
                                });
        if (extendable) {
            ranges.back() = static_cast<ImWchar>(code);
        } else {
            ranges.push_back(static_cast<ImWchar>(code));
            ranges.push_back(static_cast<ImWchar>(code));
        }
    }
    ranges.push_back(0);
    return ranges;
}

// switch_font.ttf 缺失时的兜底：系统 CJK 字体。
const char* systemCjkFont() {
#if defined(_WIN32)
    static const char* candidates[] = {"C:/Windows/Fonts/msyh.ttc", "C:/Windows/Fonts/simhei.ttf"};
#elif defined(__APPLE__)
    static const char* candidates[] = {"/System/Library/Fonts/PingFang.ttc",
                                       "/System/Library/Fonts/Hiragino Sans GB.ttc"};
#else
    static const char* candidates[] = {"/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
                                       "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc"};
#endif
    for (const char* path : candidates) {
        if (std::filesystem::exists(path)) return path;
    }
    return nullptr;
}

} // namespace

bool loadDesignerFonts(const std::filesystem::path& fontDirectory) {
    ImGuiIO& io = ImGui::GetIO();
    const std::vector<Source> sources = {
        {fontDirectory / "switch_font.ttf", 18.0f, false, Content::Text},
        {fontDirectory / "switch_icons.ttf", 18.0f, true, Content::ButtonIcons},
        {fontDirectory / "MaterialIcons-Regular.ttf", 18.0f, true, Content::MaterialIcons},
    };

    std::vector<std::vector<std::uint32_t>> owned;
    owned.reserve(sources.size());
    for (const Source& source : sources) owned.push_back(ownedCodepoints(source.content));

    // 每个源只提供自己那份码位：排掉别的源负责的码位，避免先声明的字体把后者的字形挡掉。
    // imgui 在 AddFont 时会拷贝这份数组（见 imgui_draw.cpp 的 ImMemdup），局部生命周期足够。
    std::vector<std::vector<ImWchar>> excludes;
    excludes.reserve(sources.size());
    for (std::size_t i = 0; i < sources.size(); ++i) {
        std::vector<std::uint32_t> blocked;
        for (std::size_t j = 0; j < sources.size(); ++j) {
            if (i == j) continue;
            for (std::uint32_t code : owned[j]) {
                if (std::find(owned[i].begin(), owned[i].end(), code) == owned[i].end()) blocked.push_back(code);
            }
        }
        std::sort(blocked.begin(), blocked.end());
        blocked.erase(std::unique(blocked.begin(), blocked.end()), blocked.end());
        excludes.push_back(buildExcludeRanges(blocked, owned[i]));
    }

    bool primaryFromFile = false;
    ImFontConfig primaryCfg;
    primaryCfg.GlyphExcludeRanges = excludes[0].data();
    if (std::filesystem::exists(sources[0].path)) {
        primaryFromFile =
            io.Fonts->AddFontFromFileTTF(sources[0].path.string().c_str(), sources[0].sizePixels, &primaryCfg) != nullptr;
    }
    if (!primaryFromFile) {
        const char* systemFont = systemCjkFont();
        if (systemFont != nullptr) {
            primaryFromFile = io.Fonts->AddFontFromFileTTF(systemFont, sources[0].sizePixels) != nullptr;
        }
    }
    if (!primaryFromFile) io.Fonts->AddFontDefault();

    for (std::size_t i = 1; i < sources.size(); ++i) {
        if (!std::filesystem::exists(sources[i].path)) {
            std::fprintf(stderr, "[designer] font missing: %s\n", sources[i].path.string().c_str());
            continue;
        }
        ImFontConfig cfg;
        cfg.MergeMode = true;
        cfg.GlyphExcludeRanges = excludes[i].data();
        if (io.Fonts->AddFontFromFileTTF(sources[i].path.string().c_str(), sources[i].sizePixels, &cfg) == nullptr) {
            std::fprintf(stderr, "[designer] font merge failed: %s\n", sources[i].path.string().c_str());
        }
    }

    if (!io.Fonts->Fonts.empty()) io.FontDefault = io.Fonts->Fonts.back();
    return primaryFromFile;
}

} // namespace designer

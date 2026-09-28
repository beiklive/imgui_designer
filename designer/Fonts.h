// 设计器字体来源与私用区码位归属。
//
// 三个字体文件都在 resources/fonts/ 下：
//   switch_font.ttf             主文本字体（HOS 转出，含 CJK）
//   switch_icons.ttf            NintendoExt 按键图标（U+E0E0…U+E105）
//   MaterialIcons-Regular.ttf   谷歌 Material 图标
//
// switch_icons 的 1024 个私用区码位里有 553 个与 MaterialIcons 重叠，两侧都是实心字形
// （例如 play_arrow U+E037、save U+E161），而 imgui 合并字体时先声明的源占住码位。
// 因此按键图标与 Material 图标各按下面的码位表分配，加载时用 GlyphExcludeRanges
// 让每个字体只提供自己那份，与 GUI_DEV/framework/ui/Icons.h 的归属保持一致。
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>

namespace designer {

// NintendoExt 按键图标码位，与 GUI_DEV/framework/ui/Icons.h 的 kTable 一致。
inline constexpr std::uint32_t kButtonIconCodepoints[] = {
    0xE0E0, 0xE0E1, 0xE0E2, 0xE0E3, 0xE0E4, 0xE0E5, 0xE0E6, 0xE0E7,
    0xE0EF, 0xE0F0, 0xE0EB, 0xE0EC, 0xE0ED, 0xE0EE, 0xE104, 0xE105,
};
inline constexpr std::size_t kButtonIconCount = sizeof(kButtonIconCodepoints) / sizeof(kButtonIconCodepoints[0]);

// Material 图标码位，与 GUI_DEV/framework/ui/Icons.h 的 kMaterialTable 一致（已按码位去重）。
inline constexpr std::uint32_t kMaterialIconCodepoints[] = {
    0xE3C9, 0xE3F4, 0xEB72, 0xE322, 0xE1DB, 0xE872, 0xE16C, 0xE87D,
    0xE87E, 0xE834, 0xE162, 0xE5CD, 0xE037, 0xE8B8, 0xE923, 0xE873,
    0xE8B6, 0xE835, 0xE63E, 0xE648, 0xE161, 0xE864, 0xE8B3, 0xE2C3,
    0xE2C0, 0xE413, 0xE2C7, 0xE021, 0xEA28, 0xE338, 0xE324, 0xE149,
    0xE8FD, 0xE518, 0xE51C, 0xE8FF, 0xE900, 0xE86C, 0xE001, 0xE88E,
    0xE24D, 0xE5C4, 0xE5CA,
};
inline constexpr std::size_t kMaterialIconCount = sizeof(kMaterialIconCodepoints) / sizeof(kMaterialIconCodepoints[0]);

// 把字体加载进当前 ImGui 上下文：switch_font 作主字体，两个图标字体合并进来。
// 主字体文件缺失时退回系统 CJK 字体，再退回 ImGui 内置字体。
// 返回主字体是否来自文件。
bool loadDesignerFonts(const std::filesystem::path& fontDirectory);

} // namespace designer

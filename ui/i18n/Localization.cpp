#include "Localization.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace ui {
Localization::Localization() { installDefaults(); }

void Localization::installDefaults() {
    auto& en = catalogs_[index(Language::English)];
    auto& zh = catalogs_[index(Language::Chinese)];
    const auto add = [&](const char* key, const char* english, const char* chinese) {
        en.emplace(key, english);
        zh.emplace(key, chinese);
    };
    add("app.title", "imgui UI Designer", "imgui UI 设计器");
    add("menu.file", "File", "文件"); add("menu.load", "Load", "加载"); add("menu.save", "Save", "保存");
    add("menu.settings", "Settings", "设置"); add("settings.language", "Language", "语言");
    add("toolbar.load", "Load", "加载"); add("toolbar.save", "Save", "保存"); add("toolbar.zoom_out", "-", "-");
    add("toolbar.fit", "Fit", "适应"); add("toolbar.zoom_in", "+", "+"); add("toolbar.language", "Language", "语言");
    add("language.english", "English", "英文"); add("language.chinese", "Chinese", "中文");
    add("panel.hierarchy", "Hierarchy", "层级"); add("panel.canvas", "Canvas", "画布"); add("panel.inspector", "Inspector", "检查器");
    add("panel.controls", "Controls", "控件"); add("controls.new_canvas", "New canvas", "新建画布"); add("controls.primitives", "Primitives", "基础控件");
    add("controls.add_box", "Add Box", "添加 Box"); add("controls.add_text", "Add Text", "添加 Text"); add("controls.add_label", "Add Label", "添加 Label");
    add("controls.add_image", "Add Image", "添加 Image"); add("controls.import", "Import component JSON", "导入控件 JSON"); add("controls.import_button", "Import", "导入");
    add("tabs.close", "Close", "关闭"); add("tabs.new", "New", "新建"); add("tabs.run", "Run", "运行"); add("tabs.pause", "Pause", "暂停");
    add("tabs.design", "Design", "设计"); add("tabs.running", "Running", "运行中"); add("controls.delete", "Delete", "删除");
    add("error.import", "Could not import component", "无法导入控件"); add("error.delete_root", "The root element cannot be deleted", "根节点不能删除");
    add("inspector.select_element", "Select an element to inspect.", "请选择一个元素进行检查。");
    add("inspector.id", "ID", "标识"); add("inspector.type", "Type", "类型"); add("inspector.layout", "Layout", "布局");
    add("inspector.mode", "Mode", "模式"); add("layout.absolute", "Absolute", "绝对"); add("layout.horizontal", "Horizontal", "水平");
    add("layout.vertical", "Vertical", "垂直"); add("layout.overlay", "Overlay", "叠加");
    add("inspector.x", "X", "X"); add("inspector.y", "Y", "Y"); add("inspector.width", "Width", "宽度");
    add("inspector.height", "Height", "高度"); add("inspector.spacing", "Spacing", "间距"); add("inspector.style", "Style", "样式");
    add("inspector.background", "Background", "背景"); add("inspector.border", "Border", "边框"); add("inspector.border_width", "Border Width", "边框宽度");
    add("inspector.radius", "Radius", "圆角"); add("inspector.shadow", "Shadow", "阴影"); add("inspector.opacity", "Opacity", "不透明度");
    add("inspector.clip", "Clip", "裁剪"); add("inspector.padding_left", "Padding Left", "左内边距"); add("inspector.padding_right", "Padding Right", "右内边距");
    add("inspector.padding_top", "Padding Top", "上内边距"); add("inspector.padding_bottom", "Padding Bottom", "下内边距");
    add("inspector.text", "Text", "文本"); add("inspector.content", "Content", "内容"); add("inspector.font_size", "Font Size", "字号");
    add("inspector.text_color", "Text Color", "文本颜色"); add("inspector.horizontal", "Horizontal", "水平对齐"); add("inspector.vertical", "Vertical", "垂直对齐");
    add("inspector.image", "Image", "图像"); add("inspector.source", "Source", "资源路径"); add("inspector.fit", "Fit", "适配");
    add("fit.contain", "Contain", "完整包含"); add("fit.cover", "Cover", "裁剪填充"); add("fit.stretch", "Stretch", "拉伸");
    add("canvas.no_selection", "No selection", "未选择");
    add("status.document", "Document", "文档"); add("status.ready", "Ready", "就绪"); add("status.shared_tree", "Shared UI tree", "共享 UI 树");
    add("status.core_independent", "Core independent of ImGui", "核心层独立于 ImGui");
    add("error.glfw_init", "GLFW initialization failed", "GLFW 初始化失败"); add("error.window", "Could not create GLFW window", "无法创建 GLFW 窗口");
    add("error.load", "Could not load document", "无法加载文档"); add("error.save", "Could not save document", "无法保存文档");
}

bool Localization::loadDirectory(const std::filesystem::path& directory) {
    bool loaded = false;
    loaded |= loadLanguageFile(Language::English, directory / "en.json");
    loaded |= loadLanguageFile(Language::Chinese, directory / "zh-CN.json");
    return loaded;
}

bool Localization::loadLanguageFile(Language language, const std::filesystem::path& file) {
    std::ifstream stream(file);
    if (!stream) return false;
    try {
        const auto json = nlohmann::json::parse(stream);
        if (!json.is_object()) return false;
        auto& catalog = catalogs_[index(language)];
        for (const auto& [key, value] : json.items()) if (value.is_string()) catalog[key] = value.get<std::string>();
        return true;
    } catch (...) { return false; }
}

std::string Localization::languageCode() const { return language_ == Language::Chinese ? "zh-CN" : "en"; }
std::string Localization::languageName(Language language) const {
    return catalogs_[index(language == Language::Chinese ? Language::Chinese : Language::English)].at(language == Language::Chinese ? "language.chinese" : "language.english");
}
std::string Localization::tr(std::string_view key) const {
    const auto current = catalogs_[index(language_)].find(std::string(key));
    if (current != catalogs_[index(language_)].end()) return current->second;
    const auto fallback = catalogs_[index(Language::English)].find(std::string(key));
    return fallback != catalogs_[index(Language::English)].end() ? fallback->second : std::string(key);
}
}

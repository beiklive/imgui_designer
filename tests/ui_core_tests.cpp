#include "ui/layout/UILayoutEngine.h"
#include "ui/parser/UIParser.h"
#include "ui/parser/UISerializer.h"
#include "ui/i18n/Localization.h"
#include <cassert>
#include <filesystem>
#include <string>

int main() {
    const auto source=nlohmann::json::parse(R"({"version":1,"root":{"type":"Box","id":"root","layout":{"mode":"horizontal","width":400,"height":80,"spacing":10},"children":[{"type":"Text","id":"label","layout":{"width":100,"height":30},"text":"Hello"},{"type":"HorizontalSpring","id":"spring"},{"type":"Text","id":"end","layout":{"width":60,"height":30},"text":"Done"}]}})");
    auto doc=ui::UIParser{}.parse(source);
    ui::UILayoutEngine{}.layout(*doc.root,{0,0,400,80});
    const auto& children=doc.root->children();
    assert(children.size()==3);
    assert(children[0]->computedRect.x==0 && children[0]->computedRect.width==100);
    assert(children[1]->computedRect.width==220);
    assert(children[2]->computedRect.x==340);
    const auto roundtrip=ui::UISerializer{}.serialize(*doc.root);
    assert(roundtrip.at("root").at("children").size()==3);

    auto demo=ui::UIParser{}.load(std::string(UI_DESIGNER_SOURCE_DIR)+"/examples/basic/basic.ui.json");
    assert(demo.root && demo.root->id=="root");
    ui::UILayoutEngine{}.layout(*demo.root,{0,0,1280,720});
    assert(demo.root->children().size()==2);
    auto saved=ui::UISerializer{}.serialize(*demo.root);
    auto reloaded=ui::UIParser{}.parse(saved);
    assert(reloaded.root->children().at(1)->children().at(1)->scrollable);
    const auto file=std::filesystem::temp_directory_path()/"imguiUIDesigner-roundtrip-test.json";
    ui::UISerializer{}.save(*demo.root,file.string(),demo.version);
    auto fromDisk=ui::UIParser{}.load(file.string());
    assert(fromDisk.root->children().at(1)->children().at(1)->scrollable);
    std::filesystem::remove(file);

    ui::Localization localization;
    assert(localization.tr("panel.canvas") == "Canvas");
    localization.setLanguage(ui::Language::Chinese);
    assert(localization.tr("panel.canvas") == "画布");
    assert(localization.languageCode() == "zh-CN");
    assert(localization.loadDirectory(std::string(UI_DESIGNER_SOURCE_DIR) + "/resources/i18n"));
    localization.setLanguage(ui::Language::English);
    assert(localization.tr("menu.file") == "File");
}

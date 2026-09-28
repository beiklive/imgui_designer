#include "../designer/DesignerApplication.h"
#include <string>

int main(int argc, char** argv) {
    designer::DesignerApplication app;
    std::string document = argc > 1 ? argv[1] : (designer::resourceRoot() / "examples/basic/basic.ui.json").string();
    if (!app.initialize(document)) { app.shutdown(); return 1; }
    const int result=app.run(); app.shutdown(); return result;
}

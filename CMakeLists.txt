cmake_minimum_required(VERSION 3.24)
project(imguiUIDesigner VERSION 0.1.0 LANGUAGES CXX)

option(IMGUI_UI_DESIGNER_BUILD_TESTS "Build the headless core tests" ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp"
   OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/single_include/nlohmann/json.hpp"
   OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/CMakeLists.txt"
   OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/stb_image.h")
    message(FATAL_ERROR "Dependencies are missing. Run: git submodule update --init --recursive")
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE third_party/json/single_include)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

add_library(imgui STATIC
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC third_party/imgui third_party/imgui/backends)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

add_library(ui_core STATIC
    ui/core/UIElement.cpp
    ui/widgets/Box.cpp
    ui/widgets/Text.cpp
    ui/widgets/Label.cpp
    ui/widgets/Image.cpp
    ui/containers/ScrollContainer.cpp
    ui/layout/UILayoutEngine.cpp
    ui/parser/UIDocument.cpp
    ui/parser/UIParser.cpp
    ui/parser/UISerializer.cpp
    ui/runtime/UIRuntime.cpp
    ui/i18n/Localization.cpp
    ui/theme/UITheme.cpp)
target_include_directories(ui_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ui_core PUBLIC nlohmann_json::nlohmann_json)

add_library(ui_renderer_imgui STATIC
    ui/renderer/imgui/ImGuiUIRenderer.cpp
    ui/renderer/imgui/ImageTextureCache.cpp)
target_include_directories(ui_renderer_imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ui_renderer_imgui PRIVATE third_party/stb)
target_link_libraries(ui_renderer_imgui PUBLIC ui_core imgui)

add_executable(imguiUIDesigner
    src/main.cpp
    designer/DesignerApplication.cpp
    designer/DesignerSettings.cpp
    designer/Canvas/DesignCanvas.cpp
    designer/Hierarchy/HierarchyPanel.cpp
    designer/Inspector/InspectorPanel.cpp
    designer/Toolbar/DesignerToolbar.cpp
    designer/Controls/ControlPalette.cpp)
target_include_directories(imguiUIDesigner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imguiUIDesigner PRIVATE ui_core ui_renderer_imgui imgui glfw OpenGL::GL)
target_compile_definitions(imguiUIDesigner PRIVATE UI_DESIGNER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(IMGUI_UI_DESIGNER_BUILD_TESTS)
    enable_testing()
    add_executable(ui_core_tests tests/ui_core_tests.cpp designer/DesignerSettings.cpp)
    target_link_libraries(ui_core_tests PRIVATE ui_core)
    target_compile_definitions(ui_core_tests PRIVATE UI_DESIGNER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME ui_core_tests COMMAND ui_core_tests)

    add_executable(stbi_decode_smoke tests/stbi_decode_smoke.cpp)
    target_include_directories(stbi_decode_smoke PRIVATE third_party/stb)
endif()

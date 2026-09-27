# imgui UI Designer

A C++20 desktop editor and the starting point for a renderer-independent imgui UI runtime. The document model, deterministic layout, parser, and runtime do not include or call Dear ImGui; ImGui is currently used only by the desktop renderer and Designer shell.

## Architecture

```text
JSON document -> UIParser -> shared UIElement tree -> UIRuntime -> UILayoutEngine
                                                     ├-> Designer hierarchy / inspector / canvas
                                                     └-> UIRenderer -> ImGuiUIRenderer -> ImDrawList
```

`ui/core` owns the tree, layout intent, style, state, and computed rectangles. `ui/layout` calculates geometry (Absolute, Horizontal, Vertical, Overlay, springs, and scroll content). `ui/parser` loads and saves versioned JSON. `ui/runtime` coordinates update/layout/render. `ui/renderer` is a backend boundary; the ImGui implementation issues draw-list primitives rather than using ImGui widgets to render document elements. The Designer edits the very same tree previewed by the runtime.

## Layout and document model

`UILayout` (requested x/y/width/height/mode) is separate from `UIRect computedRect` (layout output). The canvas design size is 1280 × 720. Horizontal and vertical layouts use preferred child sizes and distribute remaining main-axis space to matching springs. Scroll containers compute content extent and clip at render time; wheel input adjusts their offset. Absolute children can be moved/resized on the canvas; other children can be selected and edited in the Inspector.

Primitive widgets are **Box, Text, Image**. Button, Dialog, Toast, Tab, Slider, and similar controls are future composite components, not core primitives. See `ui/components/README.md`.

## Dependencies

Git submodules:

- [Dear ImGui](https://github.com/ocornut/imgui) (official upstream)
- [nlohmann/json](https://github.com/nlohmann/json) (header-only document JSON)
- [GLFW](https://github.com/glfw/glfw) (desktop window/input backend)
- [stb_image](https://github.com/nothings/stb) (renderer-side PNG/JPG/JPEG image decoding)

OpenGL is provided by the desktop platform. No Qt, Electron, web UI, scripting runtime, or shader system is used. Keep third-party submodules unmodified.

## Initialize and build

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build --config Release
```

The same CMake project is intended for Windows and macOS desktop builds; Linux can also be configured where GLFW/OpenGL development packages are available. A headless core test covers layout and JSON round-trip:

```sh
ctest --test-dir build --output-on-failure
```

When checking a local JPG/JPEG file directly, the optional smoke utility can be used after the test build:

```sh
build/stbi_decode_smoke path/to/image.jpeg
```

Run `build/imguiUIDesigner` (Windows: `build/Release/imguiUIDesigner.exe` for multi-config generators). The editor starts with `examples/basic/basic.ui.json`; pass another document path as the first argument. Enter a path in the top bar to Load or Save. Ctrl+S saves the current document.

## Example document

```json
{
  "version": 1,
  "root": {
    "type": "Box", "id": "root",
    "layout": { "mode": "vertical", "width": 1280, "height": 720 },
    "style": { "background": "#202020" },
    "children": [{ "type": "Text", "id": "title", "text": "imgui" }]
  }
}
```

Supported layout modes: `absolute`, `horizontal`, `vertical`, `overlay`. `HorizontalSpring` and `VerticalSpring` consume remaining space on their matching axis. `VerticalScroll` and `HorizontalScroll` establish clipped scrolling viewports. `examples/basic/basic.ui.json` demonstrates the primitives, a spring row, and vertical scroll; `examples/basic/vertical-scroll.ui.json` isolates scrolling.

## Designer

The desktop shell provides Toolbar (document load/save, zoom), Hierarchy (tree selection), Canvas (1280×720 scaled preview, zoom, pan, absolute move/resize), Inspector (ID, layout, dimensions, style, Text/Image properties), and status bar. Inspector and canvas write directly to the shared runtime tree. A small command/undo-redo interface is present for property edits; automatic command capture is not yet wired into every control.

## Internationalization

Designer chrome is localized through the renderer-independent `ui/i18n/Localization` service. English (`en`) and Simplified Chinese (`zh-CN`) catalogs live in `resources/i18n`; the Toolbar language selector switches language immediately for menus, panel titles, Inspector labels, Canvas/status text, and error messages. Document IDs, primitive type names, and user-authored Text/Image content remain unchanged. On startup the Designer tries a project font first, then common system CJK fonts (Microsoft YaHei/SimHei, PingFang, or Noto Sans CJK); it falls back to ImGui's default font when none is available.

## Current limits / next steps

- Image nodes load PNG/JPG/JPEG files through `stb_image`, upload them to cached OpenGL textures in the ImGui renderer, and support `contain`, `cover`, and `stretch`. Missing/unreadable files remain a labeled placeholder. Text alignment is implemented; custom fonts and wrapping are not yet included.
- Shadow uses a small multi-layer draw-list approximation. Box clip and scroll clipping are supported.
- Canvas selection handles, pan, wheel zoom, absolute move/resize are implemented. The command/undo API is not yet wired into edits, and there is no native file picker.
- Keep future renderer-specific image/shadow/font resources behind renderer interfaces. Possible later backends include NanoVG, Vulkan, Metal, and Switch; none are implemented here.

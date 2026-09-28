#!/usr/bin/env bash
# Build imgui UI Designer and assemble a runnable macOS .app bundle.
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="$root/build"
app="$root/dist/imgui UI Designer.app"
exe_name="imguiUIDesigner"
jobs="$(sysctl -n hw.ncpu)"

cmake -S "$root" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release -DIMGUI_UI_DESIGNER_BUILD_TESTS=ON
cmake --build "$build_dir" --config Release --parallel "$jobs"

rm -rf "$app"
mkdir -p "$app/Contents/MacOS" "$app/Contents/Resources"
cp "$build_dir/$exe_name" "$app/Contents/MacOS/$exe_name"
cp -R "$root/resources" "$app/Contents/Resources/resources"
cp -R "$root/examples" "$app/Contents/Resources/examples"

cat > "$app/Contents/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key><string>imgui UI Designer</string>
    <key>CFBundleDisplayName</key><string>imgui UI Designer</string>
    <key>CFBundleExecutable</key><string>imguiUIDesigner</string>
    <key>CFBundleIdentifier</key><string>dev.imguiuidesigner.app</string>
    <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>0.1.0</string>
    <key>CFBundleVersion</key><string>0.1.0</string>
    <key>LSMinimumSystemVersion</key><string>11.0</string>
    <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
PLIST

codesign --force --sign - "$app" >/dev/null 2>&1 || echo "warning: ad-hoc codesign skipped"
echo "app bundle: $app"

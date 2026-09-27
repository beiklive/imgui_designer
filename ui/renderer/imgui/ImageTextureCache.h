#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>

namespace ui {
struct ImageTexture {
    unsigned int id = 0;
    int width = 0;
    int height = 0;
};

class ImageTextureCache {
public:
    ImageTextureCache() = default;
    ~ImageTextureCache();
    ImageTextureCache(const ImageTextureCache&) = delete;
    ImageTextureCache& operator=(const ImageTextureCache&) = delete;

    const ImageTexture* load(const std::filesystem::path& path);
    void release();

private:
    std::unordered_map<std::string, ImageTexture> textures_;
};
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ImageTextureCache.h"
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <algorithm>
#include <system_error>

namespace ui {
namespace {
std::string cacheKey(const std::filesystem::path& path) {
    std::error_code error;
    const auto absolute = std::filesystem::absolute(path, error);
    return (error ? path : absolute).lexically_normal().string();
}
}

ImageTextureCache::~ImageTextureCache() { release(); }

const ImageTexture* ImageTextureCache::load(const std::filesystem::path& path) {
    if (path.empty()) return nullptr;
    const std::string key = cacheKey(path);
    if (const auto existing = textures_.find(key); existing != textures_.end()) return &existing->second;

    int width = 0, height = 0, channels = 0;
    stbi_uc* pixels = stbi_load(key.c_str(), &width, &height, &channels, 4);
    if (!pixels || width <= 0 || height <= 0) {
        if (pixels) stbi_image_free(pixels);
        return nullptr;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLint previousAlignment = 4;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // The desktop OpenGL 1.1 headers available on Windows do not expose this
    // OpenGL 1.2 constant, but the active GLFW context does support it.
    constexpr GLint clampToEdge = 0x812F;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampToEdge);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampToEdge);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);

    if (texture == 0) return nullptr;
    const auto [iterator, inserted] = textures_.emplace(key, ImageTexture{texture, width, height});
    return &iterator->second;
}

void ImageTextureCache::release() {
    for (const auto& [key, texture] : textures_) {
        if (texture.id != 0) {
            const GLuint id = texture.id;
            glDeleteTextures(1, &id);
        }
    }
    textures_.clear();
}
}

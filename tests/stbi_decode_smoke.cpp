#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdio>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: stbi_decode_smoke <image-path>\n");
        return 2;
    }
    int width = 0, height = 0, sourceChannels = 0;
    stbi_uc* pixels = stbi_load(argv[1], &width, &height, &sourceChannels, 4);
    if (!pixels) {
        std::fprintf(stderr, "stb_image: %s\n", stbi_failure_reason());
        return 1;
    }
    std::printf("decoded %dx%d source_channels=%d\n", width, height, sourceChannels);
    stbi_image_free(pixels);
    return 0;
}

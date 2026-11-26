#include "UIElements.h"
#include <iostream>
#include <stb_image.h>

GLuint loadIconTexture(const std::filesystem::path& path) {
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixels = stbi_load(
        path.string().c_str(),
        &width, &height, &channels,
        STBI_rgb_alpha
    );

    if (!pixels) {
        std::cerr << "Could not load icon: " << path
                  << ": " << stbi_failure_reason() << '\n';
        return 0;
    }

    GLint previousTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels
    );

    glBindTexture(GL_TEXTURE_2D, previousTexture);
    stbi_image_free(pixels);

    return texture;
}

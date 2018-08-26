#include "Texture.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <memory>

Texture::Texture(const std::string& path)
: loaded(false)
{
    std::cout << path << std::endl;
    load(path);
}

Texture::~Texture()
{
    glDeleteTextures(1, &texID);
}

void Texture::bind(int target) const
{
    glActiveTexture(GL_TEXTURE0+target);
    glBindTexture(GL_TEXTURE_2D, texID);
    glActiveTexture(GL_TEXTURE0);
}

void Texture::load(const std::string& path)
{
    int width, height;
    unsigned char* img = loadRaw(path.c_str(), width, height, 3);
    if (img)
    {
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(img);

        loaded = true;
    }
}

unsigned char* Texture::loadRaw(const std::string& path, int& width, int& height, int components)
{
	int bytesPerPixel;

	unsigned char* image = stbi_load(path.c_str(), &width, &height, &bytesPerPixel, components);

	if (image == nullptr)
    {
        printf("Invalid image: '%s'\n", path.c_str());
        return nullptr;
    }

	return image;
}

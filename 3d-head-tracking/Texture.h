#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture
{
public:
    Texture(const std::string& path);
    ~Texture();

    void bind(int target) const;
    bool isLoaded() const { return loaded; }

    static unsigned char* loadRaw(const std::string& path, int& width, int& height, int components);
protected:
private:
    void load(const std::string& path);

    bool loaded;
    GLuint texID;
};

#endif // TEXTURE_H

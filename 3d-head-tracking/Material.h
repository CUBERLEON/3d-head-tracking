#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

class Texture;

class Material
{
public:
    Material();
    ~Material();

    void setDiffuseTexture(const std::string& path);
    void setNormalTexture(const std::string& path);
    void setSpecularTexture(const std::string& path);
    void setOpacityTexture(const std::string& path);
    void setShininess(float shininess) { this->shininess = shininess; }

    const Texture* getDiffuseTexture();
    const Texture* getNormalTexture();
    const Texture* getSpecularTexture();
    const Texture* getOpacityTexture();
    float getShininess() const { return shininess; };

    static void loadDefaults();
    static const Texture* DefaultDiffuse;
    static const Texture* DefaultNormal;
    static const Texture* DefaultSpecular;
    static const Texture* DefaultOpacity;
protected:
private:
    float shininess;

    Texture* diffuse;
    Texture* normal;
    Texture* specular;
    Texture* opacity;
};

#endif // MATERIAL_H

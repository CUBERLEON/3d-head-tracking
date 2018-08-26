#include "Texture.h"

#include "Material.h"

Material::Material()
: shininess(4)
{
    diffuse = normal = specular = opacity = nullptr;
}

Material::~Material()
{
    delete diffuse;
    delete normal;
    delete specular;
    delete opacity;
}

void Material::setDiffuseTexture(const std::string& path)
{
    if (diffuse) delete diffuse;
    diffuse = new Texture(path);
}

void Material::setNormalTexture(const std::string& path)
{
    if (normal) delete normal;
    normal = new Texture(path);
}

void Material::setSpecularTexture(const std::string& path)
{
    if (specular) delete specular;
    specular = new Texture(path);
}

void Material::setOpacityTexture(const std::string& path)
{
    if (opacity) delete opacity;
    opacity = new Texture(path);
}

const Texture* Material::getDiffuseTexture()
{
    return diffuse && diffuse->isLoaded() ? diffuse : DefaultDiffuse;
}

const Texture* Material::getNormalTexture()
{
    return normal && normal->isLoaded() ? normal : DefaultNormal;
}

const Texture* Material::getSpecularTexture()
{
    return specular && specular->isLoaded() ? specular : DefaultSpecular;
}

const Texture* Material::getOpacityTexture()
{
    return opacity && opacity->isLoaded() ? opacity : DefaultOpacity;
}

void Material::loadDefaults()
{
    DefaultDiffuse = new Texture("textures/default_diffuse.png");
    DefaultNormal = new Texture("textures/default_normal.png");
    DefaultSpecular = new Texture("textures/default_specular.png");
    DefaultOpacity = new Texture("textures/default_opacity.png");
}

const Texture* Material::DefaultDiffuse = nullptr;
const Texture* Material::DefaultNormal = nullptr;
const Texture* Material::DefaultSpecular = nullptr;
const Texture* Material::DefaultOpacity = nullptr;

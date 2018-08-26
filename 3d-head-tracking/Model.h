#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <GL/glew.h>

class Material;

class Model
{
public:
    Model(const std::string& path);
    virtual ~Model();

    void draw(int shader);
protected:
private:
    void load(const std::string& path);

    std::vector<GLuint> vbos;
    std::vector<int> sizes;
    std::vector<int> materialIndices;

    std::vector<Material*> materials;
};


#endif // MODEL_H

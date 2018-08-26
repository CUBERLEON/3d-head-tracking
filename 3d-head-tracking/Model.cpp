#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <memory>
#include <fstream>

#include "Material.h"
#include "Texture.h"
#include "vec3.h"
#include "vec2.h"

using namespace tinyobj;
/*class MatReader:
    public MaterialReader
{
    public:
        MatReader(const std::string &mtl_basedir)
        : m_mtlBaseDir(mtl_basedir) {}
        virtual ~MatReader() {}
        virtual bool operator()(const std::string &matId,
                                std::vector<material_t> *materials,
                                std::map<std::string, int> *matMap,
                                std::string *err) {
          std::string filepath = m_mtlBaseDir + matId;

            std::unique_ptr<char[]> buffer;
            size_t length_read;

            if (!Data::loadFile(filepath, buffer, length_read))
            {
                return false;
            }

            std::istringstream s(std::string(buffer.get(), length_read));
            std::istream& matIStream(s);

          std::string warning;
          LoadMtl(matMap, materials, &matIStream, &warning);

          if (!warning.empty()) {
            if (err) {
              (*err) += warning;
            }
          }

          return true;
        }

    private:
        std::string m_mtlBaseDir;
};*/

Model::Model(const std::string& path)
{
    load(path);
}

Model::~Model()
{
    for (int i = 0; i < vbos.size(); ++i)
        glDeleteBuffers(1, &vbos[i]);
    for (int i = 0; i < materials.size(); ++i)
        delete materials[i];
}

void Model::draw(int shader)
{
    glEnableVertexAttribArray(0); //enabling vertex arrays
    glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);

    for (int i = 0; i < vbos.size(); ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), (void*)(0)); //specifying locations of vertices, texture coords and normals in buffer
    	glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(GLfloat), (void*)(sizes[i]*3*sizeof(GLfloat)));
    	//glVertexAttribPointer(2, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), (void*)(sizes[i]*6*sizeof(GLfloat)));
    	//glVertexAttribPointer(3, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), (void*)(sizes[i]*9*sizeof(GLfloat)));

        materials[ materialIndices[i] ]->getDiffuseTexture()->bind(0);
        //materials[ materialIndices[i] ]->getNormalTexture()->bind(1);
        //materials[ materialIndices[i] ]->getSpecularTexture()->bind(2);
        //materials[ materialIndices[i] ]->getOpacityTexture()->bind(3);

        glUniform1i(glGetUniformLocation(shader, "diffuseMap"), 0);
        //glUniform1i(glGetUniformLocation(shader, "normalMap"), 1);
        //glUniform1i(glGetUniformLocation(shader, "specularMap"), 2);
        //glUniform1i(glGetUniformLocation(shader, "opacityMap"), 3);

        //glUniform1f(glGetUniformLocation(shader, "materialShininess"), materials[ materialIndices[i] ]->getShininess());

        glDrawArrays(GL_TRIANGLES, 0, sizes[i]); //drawing data
    }

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);
}

void Model::load(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    std::string folderPath = path.substr(0, path.find_last_of("/\\")+1);

    std::ifstream file(path);

    /*std::unique_ptr<char[]> buffer;
    size_t length_read;
    if (!Data::loadFile(path, buffer, length_read))
    {
        exit(1);
    }
    std::istringstream s(std::string(buffer.get(), length_read));
    std::istream& stream(s);*/

    //MaterialFileReader materialReader(folderPath);

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), folderPath.c_str(), true))
    //if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &file, &materialReader, true))
    {
        std::cout << "Failed to load model " << path << std::endl << err << std::endl;
        exit(1);
    }
    std::cout << err;

    std::vector<bool> usedMaterials(materials.size());

    //calculating normals and tangents
    std::vector<vec3> normals(attrib.vertices.size()), tangents(attrib.vertices.size());
    for (int s = 0; s < shapes.size(); s++)
    {
        int index_offset = 0;
        for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            tinyobj::index_t ind1 = shapes[s].mesh.indices[index_offset + 0], ind2 = shapes[s].mesh.indices[index_offset + 1], ind3 = shapes[s].mesh.indices[index_offset + 2];

            vec3 v1(attrib.vertices[3*ind1.vertex_index+0], attrib.vertices[3*ind1.vertex_index+1], attrib.vertices[3*ind1.vertex_index+2]),
                 v2(attrib.vertices[3*ind2.vertex_index+0], attrib.vertices[3*ind2.vertex_index+1], attrib.vertices[3*ind2.vertex_index+2]),
                 v3(attrib.vertices[3*ind3.vertex_index+0], attrib.vertices[3*ind3.vertex_index+1], attrib.vertices[3*ind3.vertex_index+2]);

            vec2 t1(attrib.texcoords[2*ind1.texcoord_index+0], 1-attrib.texcoords[2*ind1.texcoord_index+1]),
                 t2(attrib.texcoords[2*ind2.texcoord_index+0], 1-attrib.texcoords[2*ind2.texcoord_index+1]),
                 t3(attrib.texcoords[2*ind3.texcoord_index+0], 1-attrib.texcoords[2*ind3.texcoord_index+1]);

            vec3 e1 = v2-v1, e2 = v3-v1;
            vec2 uv1 = t2-t1, uv2 = t3-t1;

            //calculate normals if model doesn't contain them
            if (attrib.normals.empty())
            {
                vec3 normal = vec3::normalize(vec3::cross(e1, e2));
                normals[ind1.vertex_index] += normal;
                normals[ind2.vertex_index] += normal;
                normals[ind3.vertex_index] += normal;
            }

            //calculate tangents
            float r = 1.f / (uv1.getX() * uv2.getY() - uv1.getY() * uv2.getX() + 1e-7f);
			vec3 tangent = (e1 * uv2.getY() - e2 * uv1.getY()) * r;

            tangents[ind1.vertex_index] += tangent;
            tangents[ind2.vertex_index] += tangent;
            tangents[ind3.vertex_index] += tangent;

            index_offset += 3;
        }
    }
    for (int i = 0; i < attrib.vertices.size(); ++i)
    {
        normals[i] = vec3::normalize(normals[i]);
        tangents[i] = vec3::normalize(tangents[i]);
    }

    //loading data
    for (int s = 0; s < shapes.size(); s++)
    {
        int index_offset = 0;

        std::vector< std::vector<GLfloat> > shapeVertices, shapeNormals, shapeTexCoords, shapeTangents;
        std::vector<int> shapeMaterials;
        int count = 0;
        int lastMaterial = -2;

        for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // per-face material
            int curMaterial = shapes[s].mesh.material_ids[f];
            if (curMaterial != lastMaterial)
            {
                shapeVertices.push_back({}); shapeNormals.push_back({}); shapeTexCoords.push_back({}); shapeTangents.push_back({});
                shapeMaterials.push_back(curMaterial);
                lastMaterial = curMaterial;
                count++;
            }

            //loop over vertices in the face.
            for (int v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                float vx, vy, vz, tx, ty, nx, ny, nz, tanx, tany, tanz;

                //vertex
                vx = attrib.vertices[3*idx.vertex_index+0];
                vy = attrib.vertices[3*idx.vertex_index+1];
                vz = attrib.vertices[3*idx.vertex_index+2];
                shapeVertices.back().push_back(vx); shapeVertices.back().push_back(vy); shapeVertices.back().push_back(vz);

                //normal
                if (attrib.normals.size() > 0)
                {
                    nx = attrib.normals[3*idx.normal_index+0];
                    ny = attrib.normals[3*idx.normal_index+1];
                    nz = attrib.normals[3*idx.normal_index+2];
                } else
                {
                    nx = normals[ idx.vertex_index ].getX();
                    ny = normals[ idx.vertex_index ].getY();
                    nz = normals[ idx.vertex_index ].getZ();
                }
                shapeNormals.back().push_back(nx); shapeNormals.back().push_back(ny); shapeNormals.back().push_back(nz);

                //texture coordinates
                if (attrib.texcoords.size() > 0)
                {
                    tx = attrib.texcoords[2*idx.texcoord_index+0];
                    ty = 1-attrib.texcoords[2*idx.texcoord_index+1];
                } else
                {
                    tx = ty = 0;
                }
                shapeTexCoords.back().push_back(tx); shapeTexCoords.back().push_back(ty);// shapeTexCoords.back().push_back(0);

                //tangents
                tanx = tangents[ idx.vertex_index ].getX();
                tany = tangents[ idx.vertex_index ].getY();
                tanz = tangents[ idx.vertex_index ].getZ();
                shapeTangents.back().push_back(tanx); shapeTangents.back().push_back(tany); shapeTangents.back().push_back(tanz);
            }
            index_offset += fv;
        }

        for (int i = 0; i < count; ++i)
        {
            GLuint vbo;
            int bufferSize = shapeVertices[i].size() / 3;

            if (bufferSize > 0)
            {
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);

                glBufferData(GL_ARRAY_BUFFER, bufferSize*5*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0,                            bufferSize*3*sizeof(GLfloat), shapeVertices[i].data());
                glBufferSubData(GL_ARRAY_BUFFER, bufferSize*3*sizeof(GLfloat), bufferSize*2*sizeof(GLfloat), shapeTexCoords[i].data());
                //glBufferSubData(GL_ARRAY_BUFFER, bufferSize*6*sizeof(GLfloat), bufferSize*3*sizeof(GLfloat), shapeNormals[i].data());
            	//glBufferSubData(GL_ARRAY_BUFFER, bufferSize*9*sizeof(GLfloat), bufferSize*3*sizeof(GLfloat), shapeTangents[i].data());

                vbos.push_back(vbo);
                sizes.push_back(bufferSize);
                materialIndices.push_back(shapeMaterials[i]);
                if (shapeMaterials[i] >= 0)
                    usedMaterials[ shapeMaterials[i] ] = true;
            }
        }
    }

    this->materials.resize( materials.size() );
    for (int i = 0; i < materials.size(); ++i)
    {
        if (usedMaterials[i])
        {
            this->materials[i] = new Material();

            if (!materials[i].diffuse_texname.empty())
                this->materials[i]->setDiffuseTexture(folderPath + materials[i].diffuse_texname);
            if (!materials[i].bump_texname.empty())
                this->materials[i]->setNormalTexture(folderPath + materials[i].bump_texname);
            if (!materials[i].specular_texname.empty())
                this->materials[i]->setSpecularTexture(folderPath + materials[i].specular_texname);
            if (!materials[i].alpha_texname.empty())
                this->materials[i]->setOpacityTexture(folderPath + materials[i].alpha_texname);

            // std::cout << materials[i].shininess << std::endl;
            this->materials[i]->setShininess(materials[i].shininess);
        }
    }
}

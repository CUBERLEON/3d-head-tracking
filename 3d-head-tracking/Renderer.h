#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Model;

class Renderer
{
public:
    Renderer(const int width, const int height, const std::string& window_title);
    ~Renderer();

    void update(float apha, float beta, float dist);

    GLFWwindow* getWindow() const { return m_window; }
private:
    const int m_width, m_height;
    GLFWwindow* m_window;

    GLuint m_shader;

    GLuint m_vpMatrix_id, m_mMatrix_id;
    GLuint angle_x_id;
    GLuint angle_y_id;
    GLuint z_offset_id;
    float z_offset;
    GLuint texture_id;

    GLuint m_texture;

    GLuint m_vbo_vertices;
    GLuint m_vbo_uv;

    glm::mat4 m_proj, m_view, m_model, m_viewProj;

    Model *m_teapot, *m_f16;
    //GLuint m_modelShader;
    glm::mat4 m_modelTransform;
};
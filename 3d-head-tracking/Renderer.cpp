#include "Renderer.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "Utils.h"
#include "Renderer.h"
#include "Model.h"

Renderer::Renderer(const int width, const int height, const std::string& window_title)
: m_width(width)
, m_height(height)
{
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto monitor = nullptr;
    //auto monitor = glfwGetPrimaryMonitor();
    m_window = glfwCreateWindow(m_width, m_height, window_title.c_str(), monitor, NULL);
    if (m_window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to open GLFW window");
    }
    glfwMakeContextCurrent(m_window);

    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW");

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    m_shader = loadShaders("data/textured_cube.vert", "data/textured_cube.frag");
    //m_modelShader = loadShaders("data/model.vert", "data/model.frag");

    m_vpMatrix_id = glGetUniformLocation(m_shader, "v_viewProj");
    m_mMatrix_id = glGetUniformLocation(m_shader, "v_model");

    angle_x_id = glGetUniformLocation(m_shader, "v_eyes_x");
    angle_y_id = glGetUniformLocation(m_shader, "v_eyes_y");
    z_offset_id = glGetUniformLocation(m_shader, "v_z_offset");
    float alpha = 0, beta = 0;

    texture_id = glGetUniformLocation(m_shader, "diffuseMap");
    m_texture = loadBMP("textures/uvtemplate.bmp");

    static const GLfloat g_vertex_buffer_data[] =
    {
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, //bottom
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,

        1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, //back
        1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,-1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, //left
        -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

        1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, //right
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,

        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, //top
        1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f
    };

    static const GLfloat g_uv_buffer_data[] =
    {
        1, 0, 0, 1, 1, 1, //bottom
        1, 0, 0, 0, 0, 1,

        1, 1, 0, 0, 0, 1, //back
        1, 1, 1, 0, 0, 0,

        1, 1, 1, 0, 0, 0, //left
        1, 1, 0, 0, 0, 1,

        1, 0, 0, 1, 1, 1, //right
        0, 1, 1, 0, 0, 0,

        0, 0, 0, 1, 1, 1, //top
        0, 0, 1, 1, 1, 0
    };

    glGenBuffers(1, &m_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &m_vbo_uv);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    m_teapot = new Model("models/teapot/teapot.obj");
    m_f16 = new Model("models/f16/f16.obj");
}

Renderer::~Renderer()
{
    glDeleteBuffers(1, &m_vbo_vertices);
    glDeleteBuffers(1, &m_vbo_uv);
    glDeleteProgram(m_shader);
    glDeleteTextures(1, &m_texture);

    glfwTerminate();

    delete m_teapot;
    delete m_f16;
}

void Renderer::update(float angle_x, float angle_y, float dist)
{
    float window_aspect_ratio = m_width / (float)m_height;

    //glm::mat4 Projection = glm::perspective(glm::radians(60.0f), window_aspect_ration, 0.01f, 100.f);
    m_proj = glm::frustum(-1.f * window_aspect_ratio, 1.f * window_aspect_ratio, -1.f, 1.f, dist, 100.f);
    //glm::mat4 Projection = glm::frustum(-1.f, 1.f, -1.f, 1.f, 1.f, 10.f);
    z_offset = 0.5f;
    m_view = glm::lookAt(glm::vec3(0.f, 0, z_offset + dist), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    //glm::mat4 Model = glm::scale(glm::mat4(1), glm::vec3(1.f, 1.f, 1.f));
    m_model = glm::scale(glm::mat4(1), glm::vec3(1.f * window_aspect_ratio, 1.f, 0.5f));
    m_viewProj = m_proj * m_view;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader);

    glUniformMatrix4fv(m_vpMatrix_id, 1, GL_FALSE, &m_viewProj[0][0]);
    glUniformMatrix4fv(m_mMatrix_id, 1, GL_FALSE, &m_model[0][0]);

    glUniform1f(angle_x_id, angle_x);
    glUniform1f(angle_y_id, angle_y);
    glUniform1f(z_offset_id, z_offset);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(texture_id, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uv);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 10 * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    m_modelTransform = glm::translate(glm::rotate(glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 0.25f)), glm::pi<float>() / 2.f, glm::vec3(0, 1, 0)), glm::vec3(-1.f, 0, 0));
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "v_model"), 1, GL_FALSE, &m_modelTransform[0][0]);
    m_f16->draw(m_shader);

    float time = glfwGetTime();
    m_modelTransform = glm::translate(glm::rotate(glm::scale(glm::mat4(1), glm::vec3(0.005f, 0.005f, 0.0025f)), 0.f, glm::vec3(0, 1, 0)), glm::vec3(-60.f, -200.f, 100.f));
    //m_modelTransform = glm::translate(glm::mat4(1), glm::vec3(-0.2f, -1.f, 0.f)) * glm::rotate(glm::mat4(1), std::sin(time), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(0.005f, 0.005f, 0.005f));
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "v_model"), 1, GL_FALSE, &m_modelTransform[0][0]);
    m_teapot->draw(m_shader);

    glUseProgram(0);

    glfwSwapBuffers(m_window);
}

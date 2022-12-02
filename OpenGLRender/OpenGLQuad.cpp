#include "OpenGLQuad.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include <glm/gtx/quaternion.hpp>

namespace OpenGLRender {
struct Quad::Data {
    Data(float width, float height) : width(width), height(height) {
        createQuad();
    }

    void createQuad() {
        const float w = width * 0.5;
        const float h = height * 0.5;

        //        const std::vector<float> vertexData ={
        //            // x   y   z     r     g     b
        //            -w, -h, 0.0f, 1.0f, 0.0f, 0.0f,
        //             w, -h, 0.0f, 0.0f, 1.0f, 0.0f,
        //             w,  h, 0.0f, 0.0f, 0.0f, 1.0f,
        //            -w,  h, 0.0f, 1.0f, 1.0f, 0.0f
        //        }

        const std::vector<float> vertexData = {
            // x   y   z     r     g     b
            -w, -h, 0.0f, 1.0f, 0.0f, 0.0f, w,  -h, 0.0f, 0.0f, 1.0f, 0.0f,
            w,  h,  0.0f, 0.0f, 0.0f, 1.0f, -w, h,  0.0f, 1.0f, 1.0f, 0.0f};

        //        const std::vector<unsigned int> indexData = {
        //            0u, 1u, 2u,
        //            2u, 3u, 0u
        //        };
        const std::vector<unsigned int> indexData = {0u, 1u, 2u, 2u, 3u, 0u};

        mesh = std::make_shared<Mesh>();
        mesh->writeVertexData(vertexData);
        mesh->writeIndexData(indexData);
        mesh->setAttributeDefinition(0, 3, 6 * sizeof(float), 0);
        mesh->setAttributeDefinition(1, 3, 6 * sizeof(float),
                                     3 * sizeof(float));

        const std::string vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 position;
            layout (location = 1) in vec3 color;
            uniform mat4 cameraMatrix;
            out vec4 colorIn;
            void main(void)
            {
                gl_Position = cameraMatrix * vec4(position, 1.0);
                colorIn = vec4(color, 1.0);
            }
        )";

        const std::string fragmentShaderSource = R"(
            #version 330 core
            in vec4 colorIn;
            out vec4 colorOut;
            void main(void)
            {
                colorOut = colorIn;
            }
        )";

        shader = std::make_shared<Shader>();
        shader->setVertexShader(vertexShaderSource);
        shader->setFragmentShader(fragmentShaderSource);
        shader->link();
    }

    float width = 1.0f;
    float height = 1.0f;
    glm::quat yaw;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
};

Quad::Quad(float width, float height)
    : m_data(std::make_shared<Data>(width, height)) {}

void Quad::update(float elapsed) {
    const float angleChangePerMillisecond = 180.0f / 1000.0f;
    const float angelChange = angleChangePerMillisecond * elapsed;

    m_data->yaw *=
        glm::angleAxis(glm::radians(angelChange), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Quad::render(const glm::mat4 &view, const glm::mat4 &projection) {
    const glm::mat4 model = glm::mat4_cast(m_data->yaw);
    const glm::mat4 cameraMatrix = projection * view * model;

    m_data->mesh->bind();
    m_data->shader->bind();
    m_data->shader->setUniform("cameraMatrix", cameraMatrix);
    m_data->mesh->render(GL_TRIANGLES);
    m_data->shader->release();
    m_data->mesh->release();
}

} // namespace OpenGLRender

#include "OpenGLQuad.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include <glm/gtx/quaternion.hpp>

namespace OpenGLRender {

Quad::Quad() {}

void Quad::update(float elapsed) {
    const float angleChangePerMillisecond = 180.0f / 1000.0f;
    m_currentAngle += angleChangePerMillisecond * elapsed;
    m_currentAngle =
        m_currentAngle > 360.0f ? m_currentAngle - 360.0f : m_currentAngle;
    m_model = glm::mat4(1.0f);
    m_model = glm::rotate(m_model, glm::radians(m_currentAngle),
                          glm::vec3(0.0f, 1.0f, 0.0f));
}

void Quad::render(const glm::mat4 &view, const glm::mat4 &projection) {
    const glm::mat4 cameraMatrix = projection * view * m_model;

    m_mesh->bind();
    m_shader->bind();
    m_shader->setUniform("cameraMatrix", cameraMatrix);
    m_mesh->render(GL_TRIANGLES);
    m_shader->release();
    m_mesh->release();
}

void Quad::attachTextureData(TextureIndex index, int width, int height,
                             unsigned char *data) {}

void Quad::attachTextureData(std::tuple<TextureIndex, int, int, unsigned char *> textureData)
{

}

Quad::~Quad() {}

void Quad::init() {
    const float w = m_width * 0.5;
    const float h = m_height * 0.5;

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

    m_mesh = std::make_shared<Mesh>();
    m_mesh->writeVertexData(vertexData);
    m_mesh->writeIndexData(indexData);
    m_mesh->setAttributeDefinition(0, 3, 6 * sizeof(float), 0);
    m_mesh->setAttributeDefinition(1, 3, 6 * sizeof(float), 3 * sizeof(float));

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

    m_shader = std::make_shared<Shader>();
    m_shader->setVertexShader(vertexShaderSource);
    m_shader->setFragmentShader(fragmentShaderSource);
    m_shader->link();
}

} // namespace OpenGLRender

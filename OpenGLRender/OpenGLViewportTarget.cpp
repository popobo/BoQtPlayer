#include "OpenGLViewportTarget.h"
#include "BoUtil.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"

namespace OpenGLRender {

struct ViewportTarget::Data {
    Data() {
        const float w = 1.0f;
        const float h = 1.0f;
        //        const std::vector<float> vertexData =
        //        {
        //          // x   y   z     r     g     b,    u,    v
        //            -w, -h, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        //             w, -h, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        //             w,  h, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        //            -w,  h, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        //        };
        const std::vector<float> vertexData = {
            // x   y   z     r     g     b,    u,    v
            -w,   -h,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, w,    -h,   0.0f,
            0.0f, 1.0f, 0.0f, 1.0f, 0.0f, w,    h,    0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, -w,   h,    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

        //        const std::vector<unsigned int> indexData =
        //        {
        //            0u, 1u, 2u,
        //            2u, 3u, 0u
        //        };

        const std::vector<unsigned int> indexData = {0u, 1u, 2u, 2u, 3u, 0u};

        mesh = std::make_shared<Mesh>();
        mesh->writeVertexData(vertexData);
        mesh->writeIndexData(indexData);
        mesh->setAttributeDefinition(0, 3, 8 * sizeof(float), 0);
        mesh->setAttributeDefinition(1, 3, 8 * sizeof(float),
                                     3 * sizeof(float));
        mesh->setAttributeDefinition(2, 3, 8 * sizeof(float),
                                     6 * sizeof(float));

        const std::string vertexShaderSource = R"(
                    #version 330 core
                    layout (location = 0) in vec3 position;
                    layout (location = 1) in vec3 color;
                    layout (location = 2) in vec2 texCoord;
                    out vec4 colorIn;
                    out vec2 texCoordIn;
                    void main(void)
                    {
                        gl_Position = vec4(position, 1.0);
                        colorIn = vec4(color, 1.0);
                        texCoordIn = texCoord;
                    }
       )";

        const std::string fragmentShaderSource = R"(
            #version 330 core
            uniform sampler2D tex;
            in vec4 colorIn;
            in vec2 texCoordIn;
            out vec4 colorOut;
            void main(void)
            {
                colorOut = texture(tex, texCoordIn);
            }
        )";

        shader = std::make_shared<Shader>();
        shader->setVertexShader(vertexShaderSource);
        shader->setFragmentShader(fragmentShaderSource);
        shader->link();
    }

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
};

ViewportTarget::ViewportTarget() : m_data(std::make_shared<Data>()) {}

void ViewportTarget::render(GLuint textureId) {
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, textureId));

    m_data->mesh->bind();
    m_data->shader->bind();
    m_data->shader->setUniform("tex", 0);
    m_data->mesh->render(GL_TRIANGLES);
    m_data->shader->release();
    m_data->mesh->release();

    GLCall(glBindVertexArray(0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

} // namespace OpenGLRender

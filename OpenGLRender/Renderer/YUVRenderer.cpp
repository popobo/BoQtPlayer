#include "YUVRenderer.h"
#include "BoLog.h"
#include "BoUtil.h"

namespace OpenGLRender {

namespace {

unsigned int generateTextureID() {
    unsigned int textureId = 0;

    GLCall(glGenTextures(1, &textureId));
    if (0 == textureId) {
        BO_ERROR("0 == textureId");
        return 0;
    }

    GLCall(glBindTexture(GL_TEXTURE_2D, textureId));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    return textureId;
}

} // namespace

YUVRenderer::YUVRenderer() {}

void YUVRenderer::init() {
    const float w = 1.0;
    const float h = 1.0;

    //        const std::vector<float> vertexData ={
    //            // x   y   z   u     v
    //            -w, -h, 0.0f, 0.0f, 0.0f,
    //             w, -h, 0.0f, 1.0f, 0.0f,
    //             w,  h, 0.0f, 1.0f, 1.0f,
    //            -w,  h, 0.0f, 0.0f, 1.0f
    //        }

    const std::vector<float> vertexData = {
        // x   y   z   u     v
        -w, -h, 0.0f, 0.0f, 0.0f, w,  -h, 0.0f, 1.0f, 0.0f,
        w,  h,  0.0f, 1.0f, 1.0f, -w, h,  0.0f, 0.0f, 1.0f};

    //        const std::vector<unsigned int> indexData = {
    //            0u, 1u, 2u,
    //            2u, 3u, 0u
    //        };
    const std::vector<unsigned int> indexData = {0u, 1u, 2u, 2u, 3u, 0u};

    m_mesh = std::make_shared<Mesh>();
    m_mesh->writeVertexData(vertexData);
    m_mesh->writeIndexData(indexData);
    m_mesh->setAttributeDefinition(0, 3, 5 * sizeof(float), 0);
    m_mesh->setAttributeDefinition(1, 2, 5 * sizeof(float), 3 * sizeof(float));
    m_yTextureId = generateTextureID();
    m_uTextureId = generateTextureID();
    m_vTextureId = generateTextureID();

    const std::string vertexShaderSource = R"(
                #version 330 core
                layout (location = 0) in vec3 position;
                layout (location = 1) in vec2 aTexCoord; //材质坐标

                out vec2 vTexCoord;

                void main(void)
                {
                    gl_Position = vec4(position, 1.0);
                    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
                }
            )";

    const std::string fragmentShaderSource = R"(
                #version 330 core
                out vec4 colorOut;

                uniform sampler2D yTexture;  //输入的材质(不透明灰度, 单像素)
                uniform sampler2D uTexture;
                uniform sampler2D vTexture;
                in vec2 vTexCoord; //顶点着色器传递的坐标

                void main(void)
                {
                    vec3 yuv;//vec3含有三个元素的向量
                    vec3 rgb;
                    //这变的rgb相当于yuv
                    yuv.r = texture2D(yTexture, vTexCoord).r;
                    yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
                    yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
                    rgb = mat3(1.0, 1.0, 1.0,
                               0.0, -0.39465, 2.03211,
                               1.13983, -0.58060, 0.0) * yuv;
                    colorOut = vec4(rgb, 1.0);
                }
            )";

    m_shader = std::make_shared<Shader>();
    m_shader->setVertexShader(vertexShaderSource);
    m_shader->setFragmentShader(fragmentShaderSource);
    m_shader->link();
}

void YUVRenderer::update(float elapsed) {}

void YUVRenderer::render(const glm::mat4 &view, const glm::mat4 &projection) {
    for (const auto &texdata : m_textureDatas) {
        auto [index, width, height, data] = texdata;
        attachTextureData(index, width, height, data);
    }

    m_mesh->bind();
    m_shader->bind();
    m_shader->setUniform("yTexture", (unsigned int)TextureIndex::index_0);
    m_shader->setUniform("uTexture", (unsigned int)TextureIndex::index_1);
    // main里没有使用vTexture, location会返回-1
    m_shader->setUniform("vTexture", (unsigned int)TextureIndex::index_2);
    m_mesh->render(GL_TRIANGLES);
    m_shader->release();
    m_mesh->release();

    GLCall(glBindVertexArray(0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    m_textureDatas.clear();
}

void YUVRenderer::attachTextureData(TextureIndex index, int width, int height,
                                    unsigned char *data) {
    switch (index) {
    case TextureIndex::index_0:
        GLCall(glActiveTexture(GL_TEXTURE0));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_yTextureId));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                            GL_UNSIGNED_BYTE, data));
        break;
    case TextureIndex::index_1:
        GLCall(glActiveTexture(GL_TEXTURE0 + 1));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_uTextureId));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                            GL_UNSIGNED_BYTE, data));
        break;
    case TextureIndex::index_2:
        GLCall(glActiveTexture(GL_TEXTURE0 + 2));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_vTextureId));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                            GL_UNSIGNED_BYTE, data));
        break;
    default:
        break;
    }
}

void YUVRenderer::attachTextureData(
    std::tuple<TextureIndex, int, int, unsigned char *> textureData) {
    m_textureDatas.push_back(textureData);
}

int YUVRenderer::textureNumber() { return TEXTURE_NUMBER; }

} // namespace OpenGLRender

#pragma once

#include "IOpenGLRenderer.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
namespace OpenGLRender {

class YUVRenderer : public IOpenGLRenderer {
  public:
    YUVRenderer();

    virtual void init() override;
    virtual void update(float elapsed) override;
    virtual void render(const glm::mat4 &view,
                        const glm::mat4 &projection) override;

    virtual void attachTextureData(
        std::tuple<TextureIndex, int, int, unsigned char *> textureData)
        override;

  private:
    void attachTextureData(TextureIndex index, int width, int height,
                           unsigned char *data);

  private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    GLuint m_yTextureId = 0;
    GLuint m_uTextureId = 0;
    GLuint m_vTextureId = 0;
    std::vector<std::tuple<TextureIndex, int, int, unsigned char *>>
        m_textureDatas;
};

} // namespace OpenGLRender

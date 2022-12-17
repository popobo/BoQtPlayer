#pragma once

#include "IOpenGLRenderer.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include <queue>

namespace OpenGLRender {

class YUVRenderer : public IOpenGLRenderer {
  public:
    YUVRenderer();

    virtual void init() override;
    virtual bool render() override;

    virtual int textureNumber() override;

    const static int TEXTURE_NUMBER = 3;

    virtual void addBoData(const std::shared_ptr<IBoData> &newBoData) override;

    virtual void stop() override;

  private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    GLuint m_yTextureId = 0;
    GLuint m_uTextureId = 0;
    GLuint m_vTextureId = 0;

    const static int MAX_LENGTH = 100;
    std::queue<std::shared_ptr<IBoData>> m_boDataQueue;
    bool m_stopReceiveData = false;
};

} // namespace OpenGLRender

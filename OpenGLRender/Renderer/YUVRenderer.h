#pragma once

#include "IOpenGLRenderer.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include "bo_thread_safe_queue.h"
#include <atomic>
#include <mutex>
#include <queue>

namespace OpenGLRender {

class YUVRenderer : public IOpenGLRenderer {
  public:
    YUVRenderer();

    virtual void init() override;
    virtual long renderBoData() override;

    virtual int textureNumber() override;

    const static int TEXTURE_NUMBER = 3;

    virtual void addBoData(std::shared_ptr<IBoData> &newBoData) override;

    virtual void clear() override;

  private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    GLuint m_yTextureId = 0;
    GLuint m_uTextureId = 0;
    GLuint m_vTextureId = 0;

    const static int32_t MAX_BODATA_QUEUE_SIZE = 50;
    bo_thread_safe_queue<std::shared_ptr<IBoData>> m_boData_queue{
        MAX_BODATA_QUEUE_SIZE};
};

} // namespace OpenGLRender

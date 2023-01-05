#pragma once

#include "IOpenGLRenderer.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
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

    virtual void addBoData(const std::shared_ptr<IBoData> &newBoData) override;

    virtual void stop() override;

    virtual void clear() override;

    virtual bool isSatisfied() override;

  private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    GLuint m_yTextureId = 0;
    GLuint m_uTextureId = 0;
    GLuint m_vTextureId = 0;

    const static int MAX_BODATA_QUEUE_SIZE = 100;
    const static int SATIFIED_BODATA_QUEUE_SIZE = MAX_BODATA_QUEUE_SIZE * 0.75;
    const static int UNSATIFIED_BODATA_QUEUE_SIZE = MAX_BODATA_QUEUE_SIZE * 0.5;
    std::queue<std::shared_ptr<IBoData>> m_boDataQueue;
    std::mutex m_boDataQueueMutex;

    bool m_stopReceiveData = false;
    std::atomic<bool> m_isSatisfied{false};
};

} // namespace OpenGLRender

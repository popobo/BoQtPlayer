#pragma once

#include "IOpenGLRenderer.h"
namespace OpenGLRender {

class YUVRenderer : public IOpenGLRenderer {
  public:
    YUVRenderer();

    virtual void init() override;
    virtual void update(float elapsed) override;
    virtual void render(const glm::mat4 &view,
                        const glm::mat4 &projection) override;

    virtual void attachTextureData(unsigned char *data) override;
};

} // namespace OpenGLRender

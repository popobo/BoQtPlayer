#pragma once

#include "glm/mat4x4.hpp"
#include "IOpenGLRenderer.h"
#include <memory>

namespace OpenGLRender {

class Quad : public IOpenGLRenderer {
  public:
    Quad(float width = 1.0f, float height = 1.0f);

    void update(float elapsed) override;

    void render(const glm::mat4 &view, const glm::mat4 &projection) override;

  private:
    struct Data;
    std::shared_ptr<Data> m_data;
};

} // namespace OpenGLRender

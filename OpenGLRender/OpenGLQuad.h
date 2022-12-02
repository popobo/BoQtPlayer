#pragma once

#include "glm/mat4x4.hpp"
#include <memory>

namespace OpenGLRender {

class Quad {
  public:
    Quad(float width = 1.0f, float height = 1.0f);

    void update(float elapsed);

    void render(const glm::mat4 &view, const glm::mat4 &projection);

  private:
    struct Data;
    std::shared_ptr<Data> m_data;
};

} // namespace OpenGLRender

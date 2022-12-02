#pragma once

#include "glad/glad.h"
#include <memory>

namespace OpenGLRender {

class ViewportTarget {
  public:
    ViewportTarget();

    void render(GLuint textureId);

  private:
    struct Data;
    std::shared_ptr<Data> m_data;
};
} // namespace OpenGLRender

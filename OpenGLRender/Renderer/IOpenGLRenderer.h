#pragma once

#include "glm/mat4x4.hpp"

namespace OpenGLRender {

class IOpenGLRenderer {
  public:
    IOpenGLRenderer(){};

    virtual void update(float elapsed) = 0;

    virtual void render(const glm::mat4 &view, const glm::mat4 &projection) = 0;

    ~IOpenGLRenderer(){};
};

} // namespace OpenGLRender

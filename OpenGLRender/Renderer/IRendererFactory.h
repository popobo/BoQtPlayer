#pragma once

#include "IOpenGLRenderer.h"
#include <memory>

namespace OpenGLRender {

class IRendererFactory {
  public:
    IRendererFactory() {}
    virtual std::shared_ptr<IOpenGLRenderer> createOpenGLRender() = 0;
    virtual ~IRendererFactory() {}
};

} // namespace OpenGLRender

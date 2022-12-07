#pragma once

#include "IRendererFactory.h"

namespace OpenGLRender {

class OpenGLQuadFactory : public IRendererFactory {
  public:
    OpenGLQuadFactory();

    virtual std::shared_ptr<IOpenGLRenderer> createOpenGLRender() override;
};

} // namespace OpenGLRender

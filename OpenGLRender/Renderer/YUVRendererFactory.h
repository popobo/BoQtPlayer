#pragma once

#include "IRendererFactory.h"

namespace OpenGLRender {

class YUVRendererFactory : public IRendererFactory {
  public:
    YUVRendererFactory();

    virtual std::shared_ptr<IOpenGLRenderer> createOpenGLRender() override;
};

} // namespace OpenGLRender

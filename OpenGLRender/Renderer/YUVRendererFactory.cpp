#include "YUVRendererFactory.h"
#include "YUVRenderer.h"

namespace OpenGLRender {

YUVRendererFactory::YUVRendererFactory() {}

std::shared_ptr<IOpenGLRenderer> YUVRendererFactory::createOpenGLRender() {
    return std::make_shared<YUVRenderer>();
}

} // namespace OpenGLRender

#include "OpenGLQuadFactory.h"
#include "OpenGLQuad.h"

namespace OpenGLRender {

OpenGLQuadFactory::OpenGLQuadFactory() {}

std::shared_ptr<IOpenGLRenderer> OpenGLQuadFactory::createOpenGLRender() {
    return std::make_shared<Quad>();
}

} // namespace OpenGLRender

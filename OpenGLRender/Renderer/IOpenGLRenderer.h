#pragma once

#include "Data/IBoData.h"
#include "glm/mat4x4.hpp"

#include <memory>
#include <tuple>

namespace OpenGLRender {

enum class TextureIndex : unsigned int {
    index_0 = 0,
    index_1,
    index_2,
    index_3
};

class IOpenGLRenderer {
  public:
    IOpenGLRenderer() {}

    ~IOpenGLRenderer() {}

    virtual void init() {}

    virtual void update(float elapsed) {}

    virtual bool render(const glm::mat4 &view, const glm::mat4 &projection) {
        return true;
    }

    virtual bool render() { return true; }

    virtual long renderBoData() { return 0; }

    virtual int textureNumber() { return 0; }

    virtual void addBoData(const std::shared_ptr<IBoData> &newBoData) {}

    virtual void stop(){};
};

} // namespace OpenGLRender

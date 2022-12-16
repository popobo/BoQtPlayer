#pragma once

#include "glm/mat4x4.hpp"

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
    IOpenGLRenderer(){};

    virtual void init() = 0;

    virtual void update(float elapsed) = 0;

    virtual void render(const glm::mat4 &view = glm::mat4(1.0),
                        const glm::mat4 &projection = glm::mat4(1.0)) = 0;

    virtual int textureNumber() = 0;

    virtual void attachTextureData(
        std::tuple<TextureIndex, int, int, unsigned char *> textureData) = 0;

    ~IOpenGLRenderer(){};
};

} // namespace OpenGLRender

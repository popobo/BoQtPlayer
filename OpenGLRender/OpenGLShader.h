#pragma once

#include "glm/mat4x4.hpp"
#include <memory>
#include <string>

namespace OpenGLRender {

class Shader {
  public:
    Shader();

    void setVertexShader(const std::string &vertexShader);

    void setFragmentShader(const std::string &fragmentShader);

    void link();

    void bind();

    void release();

    void setUniform(const std::string &name, int i);

    void setUniform(const std::string &name, const glm::mat4 &matrix);

  private:
    struct Data;
    std::shared_ptr<Data> m_data;
};

} // namespace OpenGLRender

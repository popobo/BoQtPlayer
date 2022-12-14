#pragma once

#include "IOpenGLRenderer.h"
#include "OpenGLMesh.h"
#include "OpenGLShader.h"
#include "glm/mat4x4.hpp"
#include <memory>

namespace OpenGLRender {

class Quad : public IOpenGLRenderer {
  public:
    Quad();

    virtual void init() override;

    virtual void update(float elapsed) override;

    virtual bool render(const glm::mat4 &view,
                        const glm::mat4 &projection) override;

    ~Quad();

  private:
    float m_width = 1.0f;
    float m_height = 1.0f;
    glm::mat4 m_model{1.0};
    float m_currentAngle = 0.0;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
};

} // namespace OpenGLRender

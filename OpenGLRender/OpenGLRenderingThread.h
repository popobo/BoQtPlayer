#pragma once

#include "OpenGLRenderWidget.h"
#include <QThread>
#include <memory>

namespace OpenGLRender {

class RenderingThread : public QThread {
  public:
    RenderingThread(OpenGLRenderWidget *widget);

    void stop();

    void lock();

    void unlock();

    bool isInitialized();

    GLuint framebufferTexture() const;

    bool isCurrentFramePainted() const;

    void setCurrentFramePainted(bool rendered);

  protected:
    void run() override;

  public:
    struct Data;
    std::shared_ptr<Data> data;
};

} // namespace OpenGLRender

#pragma once

#include "OpenGLWidget.h"
#include <QThread>
#include <memory>

namespace OpenGLRender {

class RenderingThread : public QThread {
  public:
    RenderingThread(Widget *widget);

    void stop();

    void lock();

    void unlock();

    bool isInitialized();

    GLuint framebufferTexture() const;

  protected:
    void run() override;

  public:
    struct Data;
    std::shared_ptr<Data> data;
};

} // namespace OpenGLRender

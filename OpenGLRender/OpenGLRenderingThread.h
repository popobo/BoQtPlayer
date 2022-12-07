#pragma once

#include "ElapsedTimer.h"
#include "OpenGLRenderWidget.h"
#include "Renderer/IOpenGLRenderer.h"
#include <QMutex>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QThread>
#include <memory>

namespace OpenGLRender {

class RenderingThread : public QThread {
  public:
    RenderingThread(std::shared_ptr<OpenGLRenderWidget> widget);

    void stop();

    void lock();

    void unlock();

    bool isInitialized();

    GLuint framebufferTexture() const;

    bool isCurrentFramePainted() const;

    void setCurrentFramePainted(bool rendered);

    void initialize();

    void renderFrame();

  protected:
    void run() override;

  private:
    std::shared_ptr<QOpenGLContext> m_context;
    std::shared_ptr<QOffscreenSurface> m_surface;
    std::weak_ptr<OpenGLRenderWidget> m_widget;
    QSize m_framebufferSize;
    QMutex m_mutex;
    bool m_exiting = false;
    bool m_initialized = false;
    bool m_isCurrentFramePainted = true;
    ElapsedTimer m_timer;
    std::shared_ptr<IOpenGLRenderer> m_renderer;

    GLuint m_framebufferTextureId = 0;
    std::shared_ptr<QOpenGLFramebufferObject> m_renderFramebufferObject;
    std::shared_ptr<QOpenGLFramebufferObject> m_displayFramebufferObject;
};

} // namespace OpenGLRender

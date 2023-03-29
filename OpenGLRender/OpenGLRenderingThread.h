#pragma once

#include "Data/IBoData.h"
#include "ElapsedTimer.h"
#include "Renderer/IOpenGLRenderer.h"
// 不include "glad/glad.h"会一直报OpenGL header already included, remove this
// include, glad already provides it 因为#include <QOpenGLContext>和#include
// <QOpenGLFramebufferObject>会把gl.h引入
#include "glad/glad.h"
#include <QMutex>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QThread>
#include <memory>
#include <queue>

namespace OpenGLRender {

class RenderingThread : public QThread {
  public:
    RenderingThread();

    void stop();

    void lock();

    void unlock();

    GLuint framebufferTexture() const;

    bool isCurrentFramePainted() const;

    void setCurrentFramePainted(bool rendered);

    bool renderFrame();

    void setTriggerPaintFunc(const std::function<void()> &func);

    void setRenderer(const std::shared_ptr<IOpenGLRenderer> &renderer);

    void setOpenGLContext(QOpenGLContext *context);

    void setFrameBufferSize(const QSize &size);

    bool isInitialized();

    void addBoData(const std::shared_ptr<IBoData> &newBoData);

    void setSyncAudioPts(long newSyncAudioPts);

    void pause();

    bool isPaused();

    void resume();
    
    void clear();

  protected:
    void run() override;
    // this method should be called between
    // QOpenGLContext->makeCurrent和QOpenGLContext->doneCurrent in correct
    // thread
    void initialize();

  private:
    std::shared_ptr<QOpenGLContext> m_context;
    std::shared_ptr<QOffscreenSurface> m_surface;
    std::shared_ptr<IOpenGLRenderer> m_renderer;
    std::shared_ptr<QOpenGLFramebufferObject> m_renderFramebufferObject;
    std::shared_ptr<QOpenGLFramebufferObject> m_displayFramebufferObject;

    QSize m_framebufferSize;
    QMutex m_mutex;
    ElapsedTimer m_timer;
    GLuint m_framebufferTextureId = 0;

    bool m_exiting = false;
    bool m_initialized = false;
    std::atomic<bool> m_isCurrentFramePainted = true;
    std::atomic<bool> m_isPaused{ false };

    std::function<void()> m_triggerPaintGLFunc;

    std::atomic<long> m_syncAudioPts = 0;
    long m_currenRenderFramePts = 0;
};

} // namespace OpenGLRender

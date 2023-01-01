#include "OpenGLRenderingThread.h"
#include "BoLog.h"
#include "BoUtil.h"
#include "ElapsedTimer.h"
#include "glm/gtx/transform.hpp"
#include <QImage>

namespace OpenGLRender {

void RenderingThread::initialize() {
    if (m_initialized) {
        return;
    }
    if (!gladLoadGL()) {
        BO_ERROR("Failed to initlized glad");
        return;
    }

    // Create the framebuffer objects. Two framebuffers is needed
    // for double-buffering.
    if (!m_renderer) {
        BO_ERROR("m_renderer is nullptr");
        return;
    }

    m_renderer->init();

    QOpenGLFramebufferObjectFormat framebufferFormat;

    framebufferFormat.setAttachment(
        QOpenGLFramebufferObject::CombinedDepthStencil);
    m_renderFramebufferObject = std::make_shared<QOpenGLFramebufferObject>(
        m_framebufferSize, framebufferFormat);

    m_displayFramebufferObject = std::make_shared<QOpenGLFramebufferObject>(
        m_framebufferSize, framebufferFormat);
}

void RenderingThread::setSyncAudioPts(long newSyncAudioPts) {
    m_syncAudioPts = newSyncAudioPts;
}

void RenderingThread::pause()
{
    m_isPaused = true;
}

bool RenderingThread::isPaused()
{
    return m_isPaused;
}

void RenderingThread::resume()
{
    m_isPaused = false;
}

void RenderingThread::clear()
{
    if (m_renderer) {
        m_renderer->clear();
    }
}

bool RenderingThread::isSatisfied()
{
    return m_renderer->isSatisfied();
}

// 只能再run()中调用，且注意线程安全
bool RenderingThread::renderFrame() {
    // bind the framebuffer for rendering
    m_renderFramebufferObject->bind();

    // set the viewport
    const QSize size = m_framebufferSize;
    GLCall(glViewport(0, 0, size.width(), size.height()));

    // Render the quad
    if (!m_renderer) {
        BO_ERROR("m_renderer is nullptr");
        return false;
    }
    m_renderer->update(m_timer.elapsed());

    m_currenRenderFramePts = m_renderer->renderBoData();
    if (m_currenRenderFramePts < 0) {
        return false;
    }

    // flush the pipeline
    GLCall(glFlush());

    // Release the framebuffer
    m_renderFramebufferObject->release();
    // Take the current framebuffer texture Id
    m_framebufferTextureId = m_renderFramebufferObject->texture();
    // Swap the framebuffers for double-buffering
    std::swap(m_renderFramebufferObject, m_displayFramebufferObject);
    return true;
}

void RenderingThread::setTriggerPaintFunc(const std::function<void()> &func) {
    m_triggerPaintGLFunc = func;
}

void RenderingThread::setRenderer(
    const std::shared_ptr<IOpenGLRenderer> &renderer) {
    m_renderer = renderer;
}

void RenderingThread::setOpenGLContext(QOpenGLContext *context) {
    m_context = std::make_shared<QOpenGLContext>();
    m_context->setShareContext(context);
    m_context->setFormat(context->format());
    m_context->create();
    m_context->moveToThread(this);

    // 延迟删除，否则在关闭窗口时会报错, 具体原因有待学习
    m_surface = {new QOffscreenSurface{},
                 [](QOffscreenSurface *surface) { surface->deleteLater(); }};
    m_surface->setFormat(m_context->format());
    m_surface->create();
    m_surface->moveToThread(this);
}

void RenderingThread::setFrameBufferSize(const QSize &size) {
    m_framebufferSize = size;
}

RenderingThread::RenderingThread() {}

void RenderingThread::stop() {
    lock();
    m_exiting = true;
    if (m_renderer) {
        m_renderer->stop();
    }
    unlock();
}

void RenderingThread::lock() { m_mutex.lock(); }

void RenderingThread::unlock() { m_mutex.unlock(); }

bool RenderingThread::isInitialized() { return m_initialized; }

void RenderingThread::addBoData(const std::shared_ptr<IBoData> &newBoData) {
    m_renderer->addBoData(newBoData);
}

GLuint RenderingThread::framebufferTexture() const {
    return m_framebufferTextureId;
}

bool RenderingThread::isCurrentFramePainted() const {
    return m_isCurrentFramePainted;
}

void RenderingThread::setCurrentFramePainted(bool rendered) {
    m_isCurrentFramePainted = rendered;
}

void RenderingThread::run() {
    for (;;) {
        if (m_exiting) {
            break;
        }

        if (!m_isCurrentFramePainted) {
            QThread::msleep(1);
            continue;
        }

        if (m_isPaused) {
            QThread::msleep(1);
            continue;
        }

        // Make the OpenGL context current on offscreen surface.
        m_context->makeCurrent(m_surface.get());

        if (!m_initialized) {
            initialize();
            m_initialized = true;
        }

        QMutexLocker locker(&m_mutex);
        bool renderResult = renderFrame();
        if (renderResult) {
            setCurrentFramePainted(false);
        }
        locker.unlock();

        m_context->doneCurrent();

        // 单位ms
        while (!m_exiting && m_currenRenderFramePts - m_syncAudioPts > 0) {
            QThread::msleep(1);
        }

        // Notify UI about new frame.
        if (renderResult && m_triggerPaintGLFunc) {
            m_triggerPaintGLFunc();
        }
    }
}

} // namespace OpenGLRender

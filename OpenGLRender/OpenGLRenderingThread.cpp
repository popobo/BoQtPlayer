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

// 只能再run()中调用，且注意线程安全
void RenderingThread::renderFrame() {
    // bind the framebuffer for rendering
    m_renderFramebufferObject->bind();

    // set the viewport
    const QSize size = m_framebufferSize;
    GLCall(glViewport(0, 0, size.width(), size.height()));

    // Render the quad
    if (!m_renderer) {
        BO_ERROR("m_renderer is nullptr");
        return;
    }
    m_renderer->update(m_timer.elapsed());

    m_renderer->render();

    // flush the pipeline
    GLCall(glFlush());

    // Release the framebuffer
    m_renderFramebufferObject->release();
    // Take the current framebuffer texture Id
    m_framebufferTextureId = m_renderFramebufferObject->texture();
    // Swap the framebuffers for double-buffering
    std::swap(m_renderFramebufferObject, m_displayFramebufferObject);
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

        if (!isCurrentFramePainted()) {
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
        renderFrame();
        // 保证双缓冲机制正常运行
        setCurrentFramePainted(false);
        locker.unlock();

        m_context->doneCurrent();

        // Notify UI about new frame.
        if (m_triggerPaintGLFunc) {
            m_triggerPaintGLFunc();
        }
    }
}

} // namespace OpenGLRender

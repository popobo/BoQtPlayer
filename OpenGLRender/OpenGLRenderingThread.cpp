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

    const float aspect = float(size.width()) / float(size.height());
    // Perspective projection matrix
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);

    // View matrix
    const glm::mat4 view =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0f));

    // Clear closr buffer
    GLCall(glClearColor(0.0f, 0.0f, 0.2f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // Set rendering attributes
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDisable(GL_CULL_FACE));

    // Render the quad
    if (!m_renderer) {
        BO_ERROR("m_renderer is nullptr");
        return;
    }
    m_renderer->update(m_timer.elapsed());

    for (const auto &textureData : m_textureTuples) {
        // auto [index, width, height, data] = textureData;
        //        m_renderer->attachTextureData(
        //            std::get<0>(textureData), std::get<1>(textureData),
        //            std::get<2>(textureData), std::get<3>(textureData));
        m_renderer->attachTextureData(textureData);
    }

    m_renderer->render(view, projection);

    m_textureTuples.clear();

    // flush the pipeline
    GLCall(glFlush());

    // Release the framebuffer
    m_renderFramebufferObject->release();
    // Take the current framebuffer texture Id
    m_framebufferTextureId = m_renderFramebufferObject->texture();
    // Swap the framebuffers for double-buffering
    QImage image = m_renderFramebufferObject->toImage();
    image.save("iamge2.png", "PNG");
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
    unlock();
}

void RenderingThread::lock() { m_mutex.lock(); }

void RenderingThread::unlock() { m_mutex.unlock(); }

bool RenderingThread::isInitialized() { return m_initialized; }

void RenderingThread::addTextureData(TextureIndex index, int width, int height,
                                     unsigned char *data) {
    m_textureTuples.push_back({index, width, height, data});
}

int RenderingThread::getTextureTupleSize() { return m_textureTuples.size(); }

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

        QThread::msleep(100);
    }
}

} // namespace OpenGLRender

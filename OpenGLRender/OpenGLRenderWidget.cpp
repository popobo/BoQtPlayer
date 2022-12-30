#include "OpenGLRenderWidget.h"
#include "BoLog.h"
#include "OpenGLViewportTarget.h"

namespace OpenGLRender {

OpenGLRenderWidget::OpenGLRenderWidget(QWidget *widget)
    : QOpenGLWidget(widget) {}

OpenGLRenderWidget::OpenGLRenderWidget(
    std::shared_ptr<IRendererFactory> rendererFactory, QWidget *widget)
    : QOpenGLWidget(widget), m_rendererFactory{rendererFactory} {}

bool OpenGLRenderWidget::startThread() {
    if (m_renderingThread) {
        stopThread();
    }

    m_renderingThread = std::make_shared<RenderingThread>();

    std::weak_ptr<OpenGLRenderWidget> wself = shared_from_this();
    m_renderingThread->setTriggerPaintFunc([wself]() {
        if (auto self = wself.lock()) {
            QMetaObject::invokeMethod(self.get(), "update");
        }
    });

    m_renderingThread->setFrameBufferSize(this->size());
    m_renderingThread->setOpenGLContext(this->context());

    if (!m_rendererFactory) {
        BO_ERROR("m_rendererFactory is nullptr");
        return false;
    }
    m_renderingThread->setRenderer(m_rendererFactory->createOpenGLRender());

    m_renderingThread->start();
    return true;
}

void OpenGLRenderWidget::stopThread() {
    if (m_renderingThread && m_renderingThread->isRunning()) {
        m_renderingThread->stop();
        m_renderingThread->quit();
        m_renderingThread->wait();
        emit threadStopped();
    }
}

std::shared_ptr<IRendererFactory> OpenGLRenderWidget::getRendererFactory() {
    return m_rendererFactory;
}

void OpenGLRenderWidget::paintGL() {
    if (!m_renderingThread || !m_renderingThread->isInitialized()) {
        return;
    }

    if (!m_viewportTarget) {
        m_viewportTarget = std::make_shared<ViewportTarget>();
    }

    // 主线程
    m_renderingThread->lock();

    const GLuint textureId = m_renderingThread->framebufferTexture();
    m_viewportTarget->render(textureId);
    m_renderingThread->setCurrentFramePainted(true);

    m_renderingThread->unlock();
    // BO_INFO("m_timer.elapsed(): {0}", m_timer.elapsed());
}

void OpenGLRenderWidget::closeEvent(QCloseEvent *e) { stopThread(); }

void OpenGLRenderWidget::pause()
{
    m_renderingThread->pause();
}

void OpenGLRenderWidget::resume()
{
    m_renderingThread->resume();
}

void OpenGLRenderWidget::setSyncAudioPts(long newSyncAudioPts) {
    m_renderingThread->setSyncAudioPts(newSyncAudioPts);
}

void OpenGLRenderWidget::clear()
{
    if (m_renderingThread) {
        m_renderingThread->clear();
    }
}

bool OpenGLRenderWidget::open() { return true; }

bool OpenGLRenderWidget::start() { return startThread(); }

void OpenGLRenderWidget::stop() { stopThread(); }

void OpenGLRenderWidget::update(const std::shared_ptr<IBoData> &boData) {
    m_renderingThread->addBoData(boData);
}

} // namespace OpenGLRender

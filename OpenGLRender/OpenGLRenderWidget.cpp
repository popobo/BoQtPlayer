#include "OpenGLRenderWidget.h"
#include "BoLog.h"
#include "OpenGLViewportTarget.h"

namespace OpenGLRender {

OpenGLRenderWidget::OpenGLRenderWidget(QWidget *widget)
    : QOpenGLWidget(widget) {}

OpenGLRenderWidget::OpenGLRenderWidget(
    std::shared_ptr<IRendererFactory> rendererFactory, QWidget *widget)
    : QOpenGLWidget(widget), m_rendererFactory{rendererFactory} {}

void OpenGLRenderWidget::startThread() {
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
        return;
    }
    m_renderingThread->setRenderer(m_rendererFactory->createOpenGLRender());

    m_renderingThread->start();
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

void OpenGLRenderWidget::receiveBoData(BoData data) {
    m_boDataQueue.push(data);
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

    if (!m_boDataQueue.empty() &&
        m_renderingThread->getTextureTupleSize() == 0) {
        m_renderingThread->addTextureData(
            TextureIndex::index_0, m_boDataQueue.front().width,
            m_boDataQueue.front().height, m_boDataQueue.front().datas[0]);
        m_renderingThread->addTextureData(
            TextureIndex::index_1, m_boDataQueue.front().width / 2,
            m_boDataQueue.front().height / 2, m_boDataQueue.front().datas[1]);
        m_renderingThread->addTextureData(
            TextureIndex::index_2, m_boDataQueue.front().width / 2,
            m_boDataQueue.front().height / 2, m_boDataQueue.front().datas[2]);
        m_boDataQueue.pop();
    }

    const GLuint textureId = m_renderingThread->framebufferTexture();
    m_viewportTarget->render(textureId);
    m_renderingThread->setCurrentFramePainted(true);
    m_renderingThread->unlock();
}

void OpenGLRenderWidget::closeEvent(QCloseEvent *e) { stopThread(); }

} // namespace OpenGLRender

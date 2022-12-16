#include "OpenGLRenderWidget.h"
#include "BoLog.h"
#include "BoThread.h"
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
}

void OpenGLRenderWidget::closeEvent(QCloseEvent *e) { stopThread(); }

bool OpenGLRenderWidget::attachTextureToRenderer() {
    BO_INFO("OpenGLRenderWidget::attachTextureToRenderer");
    std::unique_lock<std::mutex> locker{m_boDataQueueMutex};
    if (!m_boDataQueue.empty() &&
        m_renderingThread->getTextureTupleSize() == 0) {
        auto boData = m_boDataQueue.front();
        auto boDataDatas = boData->datas();
        m_renderingThread->addTextureData(TextureIndex::index_0,
                                          boData->width(), boData->height(),
                                          boDataDatas[0]);
        m_renderingThread->addTextureData(TextureIndex::index_1,
                                          boData->width() / 2,
                                          boData->height() / 2, boDataDatas[1]);
        m_renderingThread->addTextureData(TextureIndex::index_2,
                                          boData->width() / 2,
                                          boData->height() / 2, boDataDatas[2]);
        m_boDataQueue.pop();
        return true;
    }
    return false;
}

bool OpenGLRenderWidget::open() { return true; }

bool OpenGLRenderWidget::start() { return startThread(); }

void OpenGLRenderWidget::stop() { stopThread(); }

void OpenGLRenderWidget::update(const std::shared_ptr<IBoData> &boData) {
    m_renderingThread->addBoData(boData);
}

} // namespace OpenGLRender

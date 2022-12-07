#include "OpenGLRenderWidget.h"
#include "BoLog.h"
#include "OpenGLRenderingThread.h"
#include "OpenGLViewportTarget.h"

namespace OpenGLRender {

struct OpenGLRenderWidget::Data {

    //    Data(QWidget *widget = nullptr) : widget(widget) {}
    //    QWidget *widget = nullptr;
    std::shared_ptr<RenderingThread> renderingThread;
    std::shared_ptr<ViewportTarget> viewportTarget;
};

OpenGLRenderWidget::OpenGLRenderWidget(QWidget *widget)
    : QOpenGLWidget(widget), m_data(std::make_shared<Data>()) {}

OpenGLRenderWidget::OpenGLRenderWidget(
    std::shared_ptr<IRendererFactory> rendererFactory, QWidget *widget)
    : QOpenGLWidget(widget),
      m_data(std::make_shared<Data>()), m_rendererFactory{rendererFactory} {}

void OpenGLRenderWidget::startThread() {
    if (m_data->renderingThread) {
        stopThread();
    }

    m_data->renderingThread =
        std::make_shared<RenderingThread>(shared_from_this());
    m_data->renderingThread->start();
}

void OpenGLRenderWidget::stopThread() {
    if (m_data->renderingThread && m_data->renderingThread->isRunning()) {
        m_data->renderingThread->stop();
        m_data->renderingThread->quit();
        m_data->renderingThread->wait();
        emit threadStopped();
    }
}

std::shared_ptr<IRendererFactory> OpenGLRenderWidget::getRendererFactory() {
    return m_rendererFactory;
}

void OpenGLRenderWidget::paintGL() {
    if (!m_data->renderingThread || !m_data->renderingThread->isInitialized()) {
        return;
    }

    if (!m_data->viewportTarget) {
        m_data->viewportTarget = std::make_shared<ViewportTarget>();
    }

    m_data->renderingThread->lock();
    const GLuint textureId = m_data->renderingThread->framebufferTexture();
    m_data->viewportTarget->render(textureId);
    m_data->renderingThread->setCurrentFramePainted(true);
    m_data->renderingThread->unlock();
}

void OpenGLRenderWidget::closeEvent(QCloseEvent *e) { stopThread(); }

} // namespace OpenGLRender

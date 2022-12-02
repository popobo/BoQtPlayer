#include "OpenGLWidget.h"
#include "OpenGLRenderingThread.h"
#include "OpenGLViewportTarget.h"

namespace OpenGLRender {

struct Widget::Data {
    std::shared_ptr<RenderingThread> renderingThread;
    std::shared_ptr<ViewportTarget> viewportTarget;
};

Widget::Widget() : m_data(std::make_shared<Data>()) {}

void Widget::startThread() {
    if (m_data->renderingThread) {
        stopThread();
    }

    m_data->renderingThread = std::make_shared<RenderingThread>(this);
    m_data->renderingThread->start();
}

void Widget::stopThread() {
    if (m_data->renderingThread && m_data->renderingThread->isRunning()) {
        m_data->renderingThread->stop();
        m_data->renderingThread->quit();
        m_data->renderingThread->wait();
    }
    m_data->renderingThread.reset();
}

void Widget::paintGL() {
    if (!m_data->renderingThread || !m_data->renderingThread->isInitialized()) {
        return;
    }

    if (!m_data->viewportTarget) {
        m_data->viewportTarget = std::make_shared<ViewportTarget>();
    }

    m_data->renderingThread->lock();
    const GLuint textureId = m_data->renderingThread->framebufferTexture();
    m_data->viewportTarget->render(textureId);
    m_data->renderingThread->unlock();
}

void Widget::closeEvent(QCloseEvent *e) { stopThread(); }

} // namespace OpenGLRender

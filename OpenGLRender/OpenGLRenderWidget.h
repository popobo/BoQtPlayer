#pragma once

#include "OpenGLRenderingThread.h"
#include "OpenGLViewportTarget.h"
#include "Renderer/IRendererFactory.h"
#include <QOpenGLWidget>
#include <memory>

namespace OpenGLRender {

class OpenGLRenderWidget
    : public QOpenGLWidget,
      public std::enable_shared_from_this<OpenGLRenderWidget> {

    Q_OBJECT

  public:
    OpenGLRenderWidget(QWidget *widget = nullptr);

    OpenGLRenderWidget(std::shared_ptr<IRendererFactory> rendererFactory,
                       QWidget *widget = nullptr);

    void startThread();

    void stopThread();

    std::shared_ptr<IRendererFactory> getRendererFactory();

  protected:
    void paintGL() override;
    void closeEvent(QCloseEvent *e) override;

  signals:
    void threadStopped();

  private:
    // std::shared_ptr的默认构造函数创建的是指向nullptr的，没必要再强调一遍
    std::shared_ptr<IRendererFactory> m_rendererFactory;
    std::shared_ptr<RenderingThread> m_renderingThread;
    std::shared_ptr<ViewportTarget> m_viewportTarget;
};

} // namespace OpenGLRender

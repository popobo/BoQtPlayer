#pragma once

#include "Renderer/IRendererFactory.h"
#include "glad/glad.h"
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
    struct Data;
    std::shared_ptr<Data> m_data;

    std::shared_ptr<IRendererFactory> m_rendererFactory{nullptr};
};

} // namespace OpenGLRender

#pragma once

#include "Data/IBoData.h"
#include "IVideoView.h"
#include "OpenGLRenderingThread.h"
#include "OpenGLViewportTarget.h"
#include "Renderer/IRendererFactory.h"
#include <QOpenGLWidget>
#include <memory>
#include <queue>

namespace OpenGLRender {

class OpenGLRenderWidget
    : public QOpenGLWidget,
      public IVideoView,
      public std::enable_shared_from_this<OpenGLRenderWidget> {

    Q_OBJECT

  public:
    OpenGLRenderWidget(QWidget *widget = nullptr);

    OpenGLRenderWidget(std::shared_ptr<IRendererFactory> rendererFactory,
                       QWidget *widget = nullptr);

    bool startThread();

    void stopThread();

    std::shared_ptr<IRendererFactory> getRendererFactory();

    // IVideoView Interface
    virtual bool open() override;

    virtual bool start() override;

    virtual void stop() override;

    virtual void pause() override;

    virtual bool isPaused() override;

    virtual void resume() override;

    // IObserver interface
    virtual void update(const std::shared_ptr<IBoData> &boData) override;

    virtual void setSyncAudioPts(long newSyncAudioPts) override;

    virtual void clear() override;

    virtual bool isSatisfied() override;

    const static int BUFFER_MAX_LEN = 1024;

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

    ElapsedTimer m_timer;
};

} // namespace OpenGLRender

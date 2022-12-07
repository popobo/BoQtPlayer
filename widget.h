#ifndef WIDGET_H
#define WIDGET_H

#include "FrameDispatcher.h"
#include "IDecoder.h"
#include "IDemux.h"
#include "OpenGLRender/OpenGLRenderWidget.h"
#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

  public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

  private:
    Ui::Widget *ui;
    std::shared_ptr<IDemux> m_demux;
    std::shared_ptr<IDecoder> m_videoDecoder;
    std::shared_ptr<IDecoder> m_audioDecoder;
    std::shared_ptr<FrameDispatcher> m_frameDispatcher;
    std::shared_ptr<OpenGLRender::OpenGLRenderWidget> m_OpenGLRenderWidget;

  private slots:
    void openFile();
};
#endif // WIDGET_H

#ifndef WIDGET_H
#define WIDGET_H

#include "IPlayer.h"
#include "OpenGLRender/OpenGLRenderWidget.h"
#include "QAudioPlayer.h"
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

    void closeWidget();

    void timerEvent(QTimerEvent* event) override;

  private:
    Ui::Widget *ui;

    std::shared_ptr<OpenGLRender::OpenGLRenderWidget> m_OpenGLRenderWidget;
    std::shared_ptr<QAudioPlayer> m_audioPlayer;

    std::shared_ptr<IPlayer> m_player;

    bool m_sliderPressed{ false };

  private slots:
    void openFile();
    void pause();
    void resume();
    void sliderPressed();
    void sliderReleased();
    void sliderMoved(int value);
};
#endif // WIDGET_H

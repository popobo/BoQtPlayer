#include "widget.h"
#include "./ui_widget.h"
#include "BoLog.h"
#include "FFQtPlayerBuilder.h"
#include "OpenGLRender/Renderer/YUVRendererFactory.h"
#include <QAudioDevice>
#include <QFileDialog>
#include <QMediaDevices>
#include <QOpenGLContext>

namespace {
    const int PROGRESS_SLIDER_UPDATE_TIME = 40;
}

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);

    if (!QOpenGLContext::supportsThreadedOpenGL()) {
        BO_INFO("Threaded OpenGL is not supported");
        return;
    }

    std::shared_ptr<IPlayerBuilder> playerBuild =
        std::make_shared<FFQtPlayerBuilder>();
    m_player = playerBuild->buildPlayer();

    // setVideoView
    std::shared_ptr<OpenGLRender::IRendererFactory> renderFactory =
        std::make_shared<OpenGLRender::YUVRendererFactory>();
    m_OpenGLRenderWidget =
        std::make_shared<OpenGLRender::OpenGLRenderWidget>(renderFactory, this);
    m_OpenGLRenderWidget->setGeometry(QRect(0, 0, 800, 600));
    m_player->setVideoView(m_OpenGLRenderWidget);

    // setAudioPlayer
    m_audioPlayer = std::make_shared<QAudioPlayer>();
    m_player->setAudioPlayer(m_audioPlayer);
    
    ui->pushButtonResume->hide();

    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->pushButtonCloseWindow, &QPushButton::clicked, this,
            &Widget::closeWidget);

    connect(ui->pushButtonPause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->pushButtonResume, SIGNAL(clicked()), this, SLOT(resume()));
    
    connect(ui->horizontalSliderPlayProgress, SIGNAL(sliderPressed()), this,
        SLOT(sliderPressed()));
    connect(ui->horizontalSliderPlayProgress, SIGNAL(sliderReleased()), this,
        SLOT(sliderReleased()));
    connect(ui->horizontalSliderPlayProgress, SIGNAL(sliderMoved(int)), this,
        SLOT(sliderMoved(int)));

    
}

Widget::~Widget() { 
    delete ui;
}

void Widget::closeWidget() {
    m_player->stop();

    QWidget::close();
}

void Widget::timerEvent(QTimerEvent* event)
{
    if (!m_player) {
        return;
    }

    if (m_sliderPressed) {
        return;
    }
    
    auto position = m_player->getPlayPos();
    ui->horizontalSliderPlayProgress->setValue((int)(position * ui->horizontalSliderPlayProgress->maximum()));
}

void Widget::pause()
{
    if (m_player) {
        m_player->pause();
        ui->pushButtonPause->hide();
        ui->pushButtonResume->show();
    }
}

void Widget::resume()
{
    if (m_player) {
        m_player->resume();
        ui->pushButtonPause->show();
        ui->pushButtonResume->hide();
    }
}

void Widget::sliderPressed() { m_sliderPressed = true; }

void Widget::sliderReleased() { 
    m_sliderPressed = false; 
    double pos = static_cast<double>(ui->horizontalSliderPlayProgress->value()) 
        / static_cast<double>(ui->horizontalSliderPlayProgress->maximum());
    m_player->seek(pos);
}

void Widget::sliderMoved(int value) {}

void Widget::openFile() {
    // 测试用代码
    QString filename = QFileDialog::getOpenFileName(nullptr, "oepn file");
    std::string stdFilename = filename.toStdString();

    m_player->open(stdFilename.c_str());
    m_player->start();
    startTimer(PROGRESS_SLIDER_UPDATE_TIME);
}

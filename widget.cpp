#include "widget.h"
#include "./ui_widget.h"
#include "BoLog.h"
#include "FFDecoder.h"
#include "FFDemux.h"
#include "FFQtPlayerBuilder.h"
#include "FFResampler.h"
#include "OpenGLRender/Renderer/OpenGLQuadFactory.h"
#include "OpenGLRender/Renderer/YUVRendererFactory.h"
#include "QAudioPlayer.h"
#include <QAudioDevice>
#include <QFileDialog>
#include <QMediaDevices>
#include <QOpenGLContext>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);

    if (!QOpenGLContext::supportsThreadedOpenGL()) {
        BO_INFO("Threaded OpenGL is not supported");
        return;
    }

    //    std::shared_ptr<OpenGLRender::IRendererFactory> renderFactory =
    //        std::make_shared<OpenGLRender::YUVRendererFactory>();

    //    m_demux = std::make_shared<FFDemux>();
    //    m_videoDecoder = std::make_shared<FFDecoder>();
    //    m_audioDecoder = std::make_shared<FFDecoder>();
    //    m_frameDispatcher = std::make_shared<FrameDispatcher>();
    //    m_OpenGLRenderWidget =
    //        std::make_shared<OpenGLRender::OpenGLRenderWidget>(renderFactory,
    //        this);
    //    m_resampler = std::make_shared<FFResampler>();
    //    m_audioPlayer = std::make_shared<QAudioPlayer>();

    //    m_OpenGLRenderWidget->setGeometry(QRect(0, 0, 800, 600));
    //    m_demux->addObs(m_videoDecoder);
    //    m_demux->addObs(m_audioDecoder);
    //    m_videoDecoder->addStrongObs(m_frameDispatcher);
    //    m_audioDecoder->addObs(m_resampler);
    //    m_resampler->addObs(m_audioPlayer);

    //    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this,
    //    SLOT(openFile())); connect(ui->pushButtonCloseWindow,
    //    &QPushButton::clicked,
    //            m_OpenGLRenderWidget.get(),
    //            &OpenGLRender::OpenGLRenderWidget::stopThread);
    //    connect(m_OpenGLRenderWidget.get(),
    //            &OpenGLRender::OpenGLRenderWidget::threadStopped, this,
    //            &Widget::closeWidget);
    //    connect(m_frameDispatcher.get(), &FrameDispatcher::sendData,
    //            m_OpenGLRenderWidget.get(),
    //            &OpenGLRender::OpenGLRenderWidget::receiveBoData);

    std::shared_ptr<IPlayerBuilder> playerBuild =
        std::make_shared<FFQtPlayerBuilder>();
    m_player = playerBuild->buildPlayer();

    std::shared_ptr<OpenGLRender::IRendererFactory> renderFactory =
        std::make_shared<OpenGLRender::YUVRendererFactory>();

    m_OpenGLRenderWidget =
        std::make_shared<OpenGLRender::OpenGLRenderWidget>(renderFactory, this);

    m_player->setVideoView(m_OpenGLRenderWidget);
}

Widget::~Widget() { delete ui; }

void Widget::closeWidget() {
    m_demux->stop();
    m_videoDecoder->stop();
    m_audioDecoder->stop();

    QWidget::close();
}

void Widget::openFile() {
    // 测试用代码
    QString filename = QFileDialog::getOpenFileName(nullptr, "oepn file");
    std::string stdFilename = filename.toStdString();

    //   m_demux->open(stdFilename.c_str());

    //    m_videoDecoder->open(m_demux->getVideoParameter());
    //    m_audioDecoder->open(m_demux->getAudioParameter());

    //    m_demux->start();
    //    m_videoDecoder->start();
    //    m_audioDecoder->start();
    //    // 启动顺序有要求
    //    m_audioPlayer->open();
    //    m_resampler->open(m_demux->getAudioParameter(),
    //                      m_audioPlayer->audioOutFormat());

    //    m_OpenGLRenderWidget->startThread();

    m_player->open(stdFilename.c_str());
    m_player->start();
}

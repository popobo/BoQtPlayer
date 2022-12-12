#include "widget.h"
#include "./ui_widget.h"
#include "BoLog.h"
#include "FFDecoder.h"
#include "FFDemux.h"
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

    std::shared_ptr<OpenGLRender::IRendererFactory> renderFactory =
        std::make_shared<OpenGLRender::YUVRendererFactory>();

    m_demux = std::make_shared<FFDemux>();
    m_videoDecoder = std::make_shared<FFDecoder>();
    m_audioDecoder = std::make_shared<FFDecoder>();
    m_frameDispatcher = std::make_shared<FrameDispatcher>();
    m_OpenGLRenderWidget =
        std::make_shared<OpenGLRender::OpenGLRenderWidget>(renderFactory, this);
    m_resampler = std::make_shared<FFResampler>();
    m_audioPlayer = std::make_shared<QAudioPlayer>();

    m_OpenGLRenderWidget->setGeometry(QRect(0, 0, 800, 600));
    m_demux->addObs(m_videoDecoder);
    m_demux->addObs(m_audioDecoder);
    m_videoDecoder->addStrongObs(m_frameDispatcher);
    m_audioDecoder->addObs(m_resampler);
    m_resampler->addObs(m_audioPlayer);

    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->pushButtonCloseWindow, &QPushButton::clicked,
            m_OpenGLRenderWidget.get(),
            &OpenGLRender::OpenGLRenderWidget::stopThread);
    connect(m_OpenGLRenderWidget.get(),
            &OpenGLRender::OpenGLRenderWidget::threadStopped, this,
            &Widget::closeWidget);
    connect(m_frameDispatcher.get(), &FrameDispatcher::sendData,
            m_OpenGLRenderWidget.get(),
            &OpenGLRender::OpenGLRenderWidget::receiveBoData);
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

    m_demux->open(stdFilename.c_str());

    m_videoDecoder->open(m_demux->getVideoParameter());
    m_audioDecoder->open(m_demux->getAudioParameter());

    auto defaultAudioOutput = QMediaDevices::defaultAudioOutput();
    auto audioFormat = defaultAudioOutput.preferredFormat();

    BoParameter audioOutputParameter;
    AudioOutputFormat audioOutputFormat;
    audioOutputFormat.sampleRate = audioFormat.sampleRate();
    audioOutputFormat.sampleChannelCount = audioFormat.channelCount();
    switch (audioFormat.sampleFormat()) {
    case QAudioFormat::SampleFormat::UInt8:
        audioOutputFormat.sampleBits = SampleBits::UInt8;
        break;
    case QAudioFormat::SampleFormat::Int16:
        audioOutputFormat.sampleBits = SampleBits::Int16;
        break;
    case QAudioFormat::SampleFormat::Int32:
        audioOutputFormat.sampleBits = SampleBits::Int32;
        break;
    case QAudioFormat::SampleFormat::Float:
        audioOutputFormat.sampleBits = SampleBits::Float;
        break;
    default:
        break;
    }
    m_audioPlayer->setAudioDevice(defaultAudioOutput);
    m_audioPlayer->setAudioOutFormat(audioFormat);
    audioOutputParameter.setAudioOutputFormat(audioOutputFormat);

    // 先保持重采样后与原本一致
    m_resampler->open(m_demux->getAudioParameter(), audioOutputParameter);

    m_demux->start();
    m_videoDecoder->start();
    m_audioDecoder->start();
    m_audioPlayer->startPlay(m_demux->getAudioParameter());

    m_OpenGLRenderWidget->startThread();
}

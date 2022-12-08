#include "widget.h"
#include "./ui_widget.h"
#include "BoLog.h"
#include "FFDecoder.h"
#include "FFDemux.h"
#include "OpenGLRender/Renderer/OpenGLQuadFactory.h"
#include "OpenGLRender/Renderer/YUVRendererFactory.h"
#include <QFileDialog>
#include <QOpenGLContext>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);

    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));

    if (!QOpenGLContext::supportsThreadedOpenGL()) {
        BO_INFO("Threaded OpenGL is not supported");
        return;
    }

    std::shared_ptr<OpenGLRender::IRendererFactory> renderFactory =
        std::make_shared<OpenGLRender::YUVRendererFactory>();
    m_OpenGLRenderWidget =
        std::make_shared<OpenGLRender::OpenGLRenderWidget>(renderFactory, this);

    m_OpenGLRenderWidget->setGeometry(QRect(0, 0, 800, 600));

    m_demux = std::make_shared<FFDemux>();
    m_videoDecoder = std::make_shared<FFDecoder>();
    m_audioDecoder = std::make_shared<FFDecoder>();
    m_frameDispatcher = std::make_shared<FrameDispatcher>();

    m_demux->addObs(m_videoDecoder);
    m_demux->addObs(m_audioDecoder);

    m_videoDecoder->addStrongObs(m_frameDispatcher);

    connect(ui->pushButtonCloseWindow, &QPushButton::clicked,
            m_OpenGLRenderWidget.get(),
            &OpenGLRender::OpenGLRenderWidget::stopThread);
    connect(m_OpenGLRenderWidget.get(),
            &OpenGLRender::OpenGLRenderWidget::threadStopped, this,
            &QWidget::close);

    connect(m_frameDispatcher.get(), &FrameDispatcher::sendData,
            m_OpenGLRenderWidget.get(),
            &OpenGLRender::OpenGLRenderWidget::receiveBoData);
}

Widget::~Widget() { delete ui; }

void Widget::openFile() {
    // 测试用代码
    QString filename = QFileDialog::getOpenFileName(nullptr, "oepn file");
    std::string stdFilename = filename.toStdString();

    m_demux->open(stdFilename.c_str());

    m_videoDecoder->open(m_demux->getVideoParameter());
    m_audioDecoder->open(m_demux->getAudioParameter());

    m_demux->start();
    m_videoDecoder->start();
    m_audioDecoder->start();

    m_OpenGLRenderWidget->startThread();
}

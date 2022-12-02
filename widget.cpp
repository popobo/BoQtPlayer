#include "widget.h"
#include "./ui_widget.h"
#include "BoLog.h"
#include "FFDecoder.h"
#include "FFDemux.h"

#include <QFileDialog>
#include <QOpenGLContext>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    if (!QOpenGLContext::supportsThreadedOpenGL()) {
        BO_INFO("Threaded OpenGL is not supported");
        return;
    }

    m_OpenGLRenderWidget = std::make_shared<OpenGLRender::Widget>();
    m_OpenGLRenderWidget->setGeometry(QRect(0, 0, 800, 600));
    m_OpenGLRenderWidget->startThread();

    m_demux = std::make_shared<FFDemux>();
    m_videoDecoder = std::make_shared<FFDecoder>();
    m_audioDecoder = std::make_shared<FFDecoder>();
    m_frameDispatcher = std::make_shared<FrameDispatcher>();

    m_demux->addObs(m_videoDecoder);
    m_demux->addObs(m_audioDecoder);

    m_videoDecoder->addStrongObs(m_frameDispatcher);
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
}

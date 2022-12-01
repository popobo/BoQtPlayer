#include "widget.h"
#include "./ui_widget.h"
#include "FFDecoder.h"
#include "FFDemux.h"
#include <QFileDialog>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    openGLWidget = new BoOpenGLWidget(this);
    openGLWidget->setObjectName("openGLWidget");
    openGLWidget->setGeometry(QRect(0, 0, 800, 600));

    m_demux = std::make_shared<FFDemux>();
    m_videoDecoder = std::make_shared<FFDecoder>();
    m_audioDecoder = std::make_shared<FFDecoder>();
    m_frameDispatcher = std::make_shared<FrameDispatcher>();

    m_demux->addObs(m_videoDecoder);
    m_demux->addObs(m_audioDecoder);

    m_videoDecoder->addStrongObs(m_frameDispatcher);

    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));

    // connect一直返回false， 因为BoOpenGLWidget忘记
    qRegisterMetaType<BoData>("BoData");
    connect(m_frameDispatcher.get(), SIGNAL(sendData(BoData)), ui->openGLWidget,
            SLOT(receiveData(BoData)));
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

#include "widget.h"
#include "./ui_widget.h"
#include "FFDecode.h"
#include "FFDemux.h"
#include <QFileDialog>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
}

Widget::~Widget() { delete ui; }

void Widget::openFile() {
    // 测试用代码
    QString filename = QFileDialog::getOpenFileName(this, "oepn file");
    std::string stdFilename = filename.toStdString();

    // IDemux *demux = new FFDemux();
    std::shared_ptr<FFDemux> demux = std::make_shared<FFDemux>();
    // FFDemux *demux = new FFDemux();
    demux->open(stdFilename.c_str());
    auto videoDecoder = std::make_shared<FFDecode>();
    videoDecoder->open(demux->getVideoParameter());

    auto audioDecoder = std::make_shared<FFDecode>();
    audioDecoder->open(demux->getAudioParameter());

    demux->addObs(videoDecoder);
    demux->addObs(audioDecoder);

    demux->start();
    videoDecoder->start();
    audioDecoder->start();

    // 这个是必须的
    boSleep(6000);
}

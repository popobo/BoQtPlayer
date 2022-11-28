#include "widget.h"
#include "./ui_widget.h"
#include "FFDemux.h"
#include <QFileDialog>
extern "C" {
#include "libavformat/avformat.h"
}

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
}

Widget::~Widget() { delete ui; }

void Widget::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, "oepn file");
    std::string stdFilename = filename.toStdString();
    FFDemux demux;
    demux.open(stdFilename.c_str());
}

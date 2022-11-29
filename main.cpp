#include "BoLog.h"
#include "widget.h"
extern "C" {
#include "libavformat/avformat.h"
}

#include <QApplication>

AVFormatContext *context = nullptr;

int main(int argc, char *argv[]) {
    BoLog::init();

    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}

#include "BoLog.h"
#include "widget.h"
extern "C" {
#include "libavformat/avformat.h"
}

#include <QApplication>

AVFormatContext *context = nullptr;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    BoLog::init();
    int ret = avformat_open_input(&context, "", 0, 0);
    if (ret != 0) {
        BO_ERROR("failed to avformat_open_input");
    }
    return a.exec();
}

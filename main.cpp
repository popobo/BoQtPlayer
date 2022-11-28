#include "widget.h"
extern "C" {
#include "libavformat/avformat.h"
}

#include <QApplication>

AVFormatContext *context = nullptr;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    int ret = avformat_open_input(&context, "", 0, 0);
    return a.exec();
}

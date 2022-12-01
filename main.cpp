#include "BoLog.h"
#include "widget.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[]) {
    BoLog::init();

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}

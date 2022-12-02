#include "BoLog.h"

#include "OpenGLRender/OpenGLWidget.h"
#include <QApplication>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <iostream>

int main(int argc, char *argv[]) {
    BoLog::init();

    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    if (!QOpenGLContext::supportsThreadedOpenGL()) {
        std::cerr << "Threaded OpenGL is not supported" << std::endl;
        return EXIT_FAILURE;
    }

    auto m_OpenGLRenderWidget = std::make_shared<OpenGLRender::Widget>();
    m_OpenGLRenderWidget->setGeometry(QRect(0, 0, 800, 600));
    m_OpenGLRenderWidget->show();
    m_OpenGLRenderWidget->startThread();

    return a.exec();
}

#include "BoOpenGLWidget.h"

BoOpenGLWidget::BoOpenGLWidget(QWidget *widget) : QOpenGLWidget(widget) {}

void BoOpenGLWidget::initializeGL() {
    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();
    functions->glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void BoOpenGLWidget::paintGL() {}

void BoOpenGLWidget::resizeGL(int width, int height) {}

#ifndef BOOPENGLWIDGET_H
#define BOOPENGLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class BoOpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions {
  public:
    BoOpenGLWidget(QWidget *widget);

    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int width, int height) override;
};

#endif // BOOPENGLWIDGET_H

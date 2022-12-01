#ifndef BOOPENGLWIDGET_H
#define BOOPENGLWIDGET_H

#include "IObserver.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>

class BoOpenGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions {
    Q_OBJECT

  public:
    BoOpenGLWidget(QWidget *widget);

    // QOpenGLWidget interface
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int width, int height) override;

    GLuint initShader(const char *sourceCode, GLint type);

    void getTexture(unsigned int index, int width, int height,
                    unsigned char *buf, bool isAlpha = false);

    void draw();

    ~BoOpenGLWidget();

    void GLClearError();
    bool GLLogCall(const char *function, const char *file, int line);

  public slots:
    void receiveData(BoData boData);

  private:
    GLuint m_VAO{0};
    GLuint m_VBO{0};
    GLuint m_vertexShader{0};
    GLuint m_fragmentShader{0};
    GLuint m_programID{0};
    GLuint m_textures[128]{0};
    QWidget *m_widget{nullptr};
};

#endif // BOOPENGLWIDGET_H

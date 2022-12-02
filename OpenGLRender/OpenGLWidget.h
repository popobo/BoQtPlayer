#pragma once

#include "glad/glad.h"
#include <QOpenGLWidget>
#include <memory>

namespace OpenGLRender {

class Widget : public QOpenGLWidget {
  public:
    Widget();

    void startThread();

    void stopThread();

  protected:
    void paintGL() override;
    void closeEvent(QCloseEvent *e) override;

  private:
    struct Data;
    std::shared_ptr<Data> m_data;
};

} // namespace OpenGLRender

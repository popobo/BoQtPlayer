#include "OpenGLRenderingThread.h"
#include "BoLog.h"
#include "BoUtil.h"
#include "ElapsedTimer.h"
#include "OpenGLQuad.h"
#include "glm/gtx/transform.hpp"
#include <QMutex>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>

namespace OpenGLRender {

struct RenderingThread::Data {
    Data(OpenGLRenderWidget *widget, const QSize &framebufferSize)
        : widget(widget), framebufferSize(framebufferSize) {}

    std::shared_ptr<QOpenGLContext> context;
    std::shared_ptr<QOffscreenSurface> surface;
    OpenGLRenderWidget *widget;
    QSize framebufferSize;
    QMutex mutex;
    bool exiting = false;
    bool initialized = false;
    bool isCurrentFramePainted = true;
    ElapsedTimer timer;
    std::shared_ptr<Quad> quad;

    GLuint framebufferTextureId = 0;
    std::shared_ptr<QOpenGLFramebufferObject> renderFramebufferObject;
    std::shared_ptr<QOpenGLFramebufferObject> displayFramebufferObject;
};

namespace {
void initialize(std::shared_ptr<RenderingThread::Data> data) {
    if (data->initialized) {
        return;
    }
    if (!gladLoadGL()) {
        BO_ERROR("Failed to initlized glad");
        return;
    }

    // Create the framebuffer objects. Two framebuffers is needed
    // for double-buffering.
    data->quad = std::make_shared<Quad>(2.0f, 2.0f);
    QOpenGLFramebufferObjectFormat framebufferFormat;

    framebufferFormat.setAttachment(
        QOpenGLFramebufferObject::CombinedDepthStencil);
    data->renderFramebufferObject = std::make_shared<QOpenGLFramebufferObject>(
        data->framebufferSize, framebufferFormat);

    data->displayFramebufferObject = std::make_shared<QOpenGLFramebufferObject>(
        data->framebufferSize, framebufferFormat);
}

void renderFrame(std::shared_ptr<RenderingThread::Data> data) {
    // bind the framebuffer for rendering
    data->renderFramebufferObject->bind();

    // set the viewport
    const QSize size = data->framebufferSize;
    GLCall(glViewport(0, 0, size.width(), size.height()));

    const float aspect = float(size.width()) / float(size.height());
    // Perspective projection matrix
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);

    // View matrix
    const glm::mat4 view =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0f));

    // Clear closr buffer
    GLCall(glClearColor(0.0f, 0.0f, 0.2f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // Set rendering attributes
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDisable(GL_CULL_FACE));

    // Render the quad
    data->quad->update(data->timer.elapsed());
    data->quad->render(view, projection);

    // flush the pipeline
    GLCall(glFlush());

    // Release the framebuffer
    data->renderFramebufferObject->release();
    // Take the current framebuffer texture Id
    data->framebufferTextureId = data->renderFramebufferObject->texture();
    // Swap the framebuffers for double-buffering
    std::swap(data->renderFramebufferObject, data->displayFramebufferObject);
}

} // namespace

RenderingThread::RenderingThread(OpenGLRenderWidget *widget)
    : data(std::make_shared<Data>(widget, widget->size())) {

    data->context = std::make_shared<QOpenGLContext>();
    data->context->setShareContext(widget->context());
    data->context->setFormat(widget->context()->format());
    data->context->create();
    data->context->moveToThread(this);

    data->surface = std::make_shared<QOffscreenSurface>();
    data->surface->setFormat(data->context->format());
    data->surface->create();
    data->surface->moveToThread(this);
}

void RenderingThread::stop() {
    lock();
    data->exiting = true;
    unlock();
}

void RenderingThread::lock() { data->mutex.lock(); }

void RenderingThread::unlock() { data->mutex.unlock(); }

bool RenderingThread::isInitialized() { return data->initialized; }

GLuint RenderingThread::framebufferTexture() const {
    return data->framebufferTextureId;
}

bool RenderingThread::isCurrentFramePainted() const {
    return data->isCurrentFramePainted;
}

void RenderingThread::setCurrentFramePainted(bool rendered) {
    data->isCurrentFramePainted = rendered;
}

void RenderingThread::run() {
    for (;;) {
        if (data->exiting) {
            break;
        }

        if (!isCurrentFramePainted()) {
            QThread::msleep(1);
            continue;
        }

        // Make the OpenGL context current on offscreen surface.
        data->context->makeCurrent(data->surface.get());

        if (!data->initialized) {
            initialize(data);
            data->initialized = true;
        }
        lock();
        renderFrame(data);
        setCurrentFramePainted(false);
        unlock();
        data->context->doneCurrent();

        // Notify UI about new frame.
        QMetaObject::invokeMethod(data->widget, "update");
        QThread::msleep(1);
    }
}

} // namespace OpenGLRender

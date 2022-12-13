#include "FFQtPlayerBuilder.h"
#include "FFDecoder.h"
#include "FFDemux.h"
#include "FFQtFrameDispathcer.h"
#include "FFResampler.h"
#include "OpenGLRenderWidget.h"
#include "QAudioPlayer.h"

FFQtPlayerBuilder::FFQtPlayerBuilder() {}

FFQtPlayerBuilder::~FFQtPlayerBuilder() {}

std::shared_ptr<IDemux> FFQtPlayerBuilder::createDemux() {
    return std::make_shared<FFDemux>();
}

std::shared_ptr<IDecoder> FFQtPlayerBuilder::createDecoder() {
    return std::make_shared<FFDecoder>();
}

std::shared_ptr<IFrameDispatcher> FFQtPlayerBuilder::createFrameDispathcer() {
    return std::make_shared<FFQtFrameDispathcer>();
}

std::shared_ptr<IResampler> FFQtPlayerBuilder::createResampler() {
    return std::make_shared<FFResampler>();
}

std::shared_ptr<IVideoView> FFQtPlayerBuilder::createVideoView() {
    return std::make_shared<OpenGLRender::OpenGLRenderWidget>();
}

std::shared_ptr<IAudioPlayer> FFQtPlayerBuilder::createAudioPlayer() {
    return std::make_shared<QAudioPlayer>();
}

std::shared_ptr<IPlayer> FFQtPlayerBuilder::createPlayer() {
    return std::make_shared<IPlayer>();
}

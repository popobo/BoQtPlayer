#include "FFQtPlayerBuilder.h"
#include "FFDecoder.h"
#include "FFDemux.h"
#include "FFResampler.h"
#include "OpenGLRenderWidget.h"
#include "FFQtPlayer.h"

FFQtPlayerBuilder::FFQtPlayerBuilder() {}

FFQtPlayerBuilder::~FFQtPlayerBuilder() {}

std::shared_ptr<IDemux> FFQtPlayerBuilder::createDemux() {
    return std::make_shared<FFDemux>();
}

std::shared_ptr<IDecoder> FFQtPlayerBuilder::createDecoder() {
    return std::make_shared<FFDecoder>();
}

std::shared_ptr<IResampler> FFQtPlayerBuilder::createResampler() {
    return std::make_shared<FFResampler>();
}

std::shared_ptr<IPlayer> FFQtPlayerBuilder::createPlayer() {
    return std::make_shared<FFQtPlayer>();
}

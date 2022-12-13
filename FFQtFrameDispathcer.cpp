#include "FFQtFrameDispathcer.h"

FFQtFrameDispathcer::FFQtFrameDispathcer() {}

void FFQtFrameDispathcer::update(const std::shared_ptr<IBoData> &boData) {
    emit sendData(boData);
}

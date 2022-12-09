#include "FrameDispatcher.h"

FrameDispatcher::FrameDispatcher() {}

void FrameDispatcher::update(const std::shared_ptr<BoData> &boData) {
    emit sendData(boData);
}

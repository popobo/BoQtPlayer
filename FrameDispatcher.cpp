#include "FrameDispatcher.h"

FrameDispatcher::FrameDispatcher() {}

void FrameDispatcher::update(const std::shared_ptr<IBoData> &boData) {
    emit sendData(boData);
}

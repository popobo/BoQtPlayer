#include "FrameDispatcher.h"

FrameDispatcher::FrameDispatcher() {}

void FrameDispatcher::update(BoData boData) { emit sendData(boData); }

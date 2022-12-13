#include "IResampler.h"

IResampler::IResampler() {}

IResampler::~IResampler() {}

void IResampler::update(const std::shared_ptr<IBoData> &boData) {
    //    if (!boData->isAudio()) {
    //        return;
    //    }
    auto boDataOut = resample(boData);
    if (boData->size() > 0) {
        notify(boDataOut);
    }
}

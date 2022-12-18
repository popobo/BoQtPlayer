#include "IParameter.h"

IParameter::IParameter() {}

double IParameter::audioTimeBase() const { return m_audioTimeBase; }

void IParameter::setAudioTimeBase(double newAudioTimeBase) {
    m_audioTimeBase = newAudioTimeBase;
}

double IParameter::videoTimeBase() const { return m_videoTimeBase; }

void IParameter::setVideoTimeBase(double newVideoTimeBase) {
    m_videoTimeBase = newVideoTimeBase;
}

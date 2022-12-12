#include "BoParameter.h"
#include "BoLog.h"

BoParameter::BoParameter() { m_aVCodecParameters = avcodec_parameters_alloc(); }

BoParameter::~BoParameter() { avcodec_parameters_free(&m_aVCodecParameters); }

BoParameter::BoParameter(const BoParameter &boParameter) {
    m_aVCodecParameters = avcodec_parameters_alloc();
    if (m_aVCodecParameters) {
        int ret = avcodec_parameters_copy(m_aVCodecParameters,
                                          boParameter.m_aVCodecParameters);
        if (ret < 0) {
            BO_ERROR("Failed to avcodec_parameters_copy");
        }
    }
    m_outAudioFormat = boParameter.m_outAudioFormat;
}

BoParameter &BoParameter::operator=(const BoParameter &boParameter) {
    if (m_aVCodecParameters) {
        avcodec_parameters_free(&m_aVCodecParameters);
    }
    m_aVCodecParameters = avcodec_parameters_alloc();
    if (m_aVCodecParameters) {
        int ret = avcodec_parameters_copy(m_aVCodecParameters,
                                          boParameter.m_aVCodecParameters);
        if (ret < 0) {
            BO_ERROR("Failed to avcodec_parameters_copy");
        }
    }
    m_outAudioFormat = boParameter.m_outAudioFormat;
    return *this;
}

AVCodecParameters *BoParameter::getPara() const { return m_aVCodecParameters; }

void BoParameter::setPara(AVCodecParameters *newPara) {
    if (m_aVCodecParameters) {
        int ret = avcodec_parameters_copy(m_aVCodecParameters, newPara);
        if (ret < 0) {
            BO_ERROR("Failed to avcodec_parameters_copy");
        }
    }
}

void BoParameter::setAudioOutputFormat(AudioOutputFormat newAudioOutFormat) {
    m_outAudioFormat = newAudioOutFormat;
}

AudioOutputFormat BoParameter::getAudioOutputFormat() const {
    return m_outAudioFormat;
}

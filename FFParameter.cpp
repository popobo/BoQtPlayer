#include "FFParameter.h"
#include "BoLog.h"
extern "C" {
#include "libavcodec/avcodec.h"
}

FFParameter::FFParameter() { m_aVCodecParameters = avcodec_parameters_alloc(); }

FFParameter::~FFParameter() {
    avcodec_parameters_free((AVCodecParameters **)&m_aVCodecParameters);
}

FFParameter::FFParameter(const FFParameter &FFParameter) {
    m_aVCodecParameters = avcodec_parameters_alloc();
    if (m_aVCodecParameters) {
        int ret = avcodec_parameters_copy(
            (AVCodecParameters *)m_aVCodecParameters,
            (AVCodecParameters *)FFParameter.m_aVCodecParameters);
        if (ret < 0) {
            BO_ERROR("Failed to avcodec_parameters_copy");
        }
    }
    m_outAudioFormat = FFParameter.m_outAudioFormat;
}

FFParameter &FFParameter::operator=(const FFParameter &FFParameter) {
    if (m_aVCodecParameters) {
        avcodec_parameters_free((AVCodecParameters **)&m_aVCodecParameters);
    }
    m_aVCodecParameters = avcodec_parameters_alloc();
    if (m_aVCodecParameters) {
        int ret = avcodec_parameters_copy(
            (AVCodecParameters *)m_aVCodecParameters,
            (AVCodecParameters *)FFParameter.m_aVCodecParameters);
        if (ret < 0) {
            BO_ERROR("Failed to avcodec_parameters_copy");
        }
    }
    m_outAudioFormat = FFParameter.m_outAudioFormat;
    return *this;
}

void *FFParameter::getPara() const { return m_aVCodecParameters; }

void FFParameter::setPara(void *newPara) {
    if (m_aVCodecParameters) {
        int ret =
            avcodec_parameters_copy((AVCodecParameters *)m_aVCodecParameters,
                                    (AVCodecParameters *)newPara);
        if (ret < 0) {
            BO_ERROR("Failed to avcodec_parameters_copy");
        }
    }
}

void FFParameter::setAudioOutputFormat(AudioOutputFormat newAudioOutFormat) {
    m_outAudioFormat = newAudioOutFormat;
}

AudioOutputFormat FFParameter::getAudioOutputFormat() const {
    return m_outAudioFormat;
}

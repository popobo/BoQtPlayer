#include "FFResampler.h"
#include "BoLog.h"
#include "Data/BoAudioData.h"

FFResampler::FFResampler() {}

FFResampler::~FFResampler() {
    if (m_swrContext) {
        swr_free(&m_swrContext);
    }
}

bool FFResampler::open(BoParameter parameterIn, BoParameter parameterOut) {
    std::unique_lock<std::mutex> locker(m_swrContextMutex);
    if (m_swrContext) {
        swr_free(&m_swrContext);
    }
    m_swrContext = swr_alloc();
    auto paraIn = parameterIn.getPara();

    auto audioOutputFormat = parameterOut.getAudioOutputFormat();
    AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE;
    switch (audioOutputFormat.sampleBits) {
    case SampleBits::UInt8:
        sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_U8;
        break;
    case SampleBits::Int16:
        sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_S16;
        break;
    case SampleBits::Int32:
        sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_S32;
        break;
    case SampleBits::Float:
        sampleFormat = AVSampleFormat::AV_SAMPLE_FMT_FLT;
        break;
    default:
        break;
    }

    AVChannelLayout channelLayout;
    channelLayout.nb_channels = audioOutputFormat.sampleChannelCount;

    int ret = swr_alloc_set_opts2(
        &m_swrContext, &channelLayout, sampleFormat,
        audioOutputFormat.sampleRate, &paraIn->ch_layout,
        (AVSampleFormat)paraIn->format, paraIn->sample_rate, 0, nullptr);
    if (ret != 0) {
        BO_ERROR("swr_alloc_set_opts2 error {0}", ret);
        return false;
    }
    ret = swr_init(m_swrContext);
    if (ret != 0) {
        BO_ERROR("swr_init falied ", ret);
        return false;
    }
    outChannels = paraIn->ch_layout.nb_channels;
    outFormat = AV_SAMPLE_FMT_S16;

    BO_INFO("FFresampler open successfully");

    return true;
}

std::shared_ptr<IBoData>
FFResampler::resample(const std::shared_ptr<IBoData> &boDataIn) {
    if (boDataIn->size() <= 0 || !boDataIn->structDataPtr()) {
        return nullptr;
    }

    std::unique_lock<std::mutex> locker(m_swrContextMutex);
    if (!m_swrContext) {
        return nullptr;
    }

    // 输出
    auto boDataOut = std::make_shared<BoAudioData>();
    AVFrame *frame = (AVFrame *)boDataIn->structDataPtr();
    int outputSize = outChannels * frame->nb_samples *
                     av_get_bytes_per_sample((AVSampleFormat)outFormat);

    if (outputSize <= 0) {
        return nullptr;
    }

    boDataOut->alloc(outputSize, nullptr);
    uint8_t *outPutArray[2] = {nullptr};
    outPutArray[0] = boDataOut->data();
    int len = swr_convert(m_swrContext, outPutArray, frame->nb_samples,
                          (const uint8_t **)frame->data, frame->nb_samples);

    if (len <= 0) {
        return nullptr;
    }

    return boDataOut;
}

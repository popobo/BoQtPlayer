#include "FFDecoder.h"
#include "BoLog.h"
#include "BoUtil.h"
extern "C" {
#include "libswscale/swscale.h"
}
#include <QImage>

FFDecoder::FFDecoder() {}

bool FFDecoder::open(const BoParameter &parameter) {
    if (!parameter.getPara()) {
        return false;
    }
    auto para = parameter.getPara();
    // 1.查找解码器
    const AVCodec *avc = avcodec_find_decoder(para->codec_id);
    if (!avc) {
        BO_INFO("avcodec_find_decoder %d failed!", para->codec_id);
    }
    BO_INFO("avcodec_find_decoder {0} successfully!", para->codec_id);
    // 2.创建解码器上下文，并复制参数
    m_codecContext = avcodec_alloc_context3(avc);
    avcodec_parameters_to_context(m_codecContext, para);

    //多线程解码
    m_codecContext->thread_count = 8;
    // 3.打开解码器
    int ret = avcodec_open2(m_codecContext, nullptr, nullptr);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    BO_INFO("avcodec_open2 successfully");

    if (AVMEDIA_TYPE_VIDEO == m_codecContext->codec_type) {
        m_isAudio = false;
    } else if (AVMEDIA_TYPE_AUDIO == m_codecContext->codec_type) {
        m_isAudio = true;
    }

    return true;
}

bool FFDecoder::sendPacket(const BoData &boData) {
    if (boData.size <= 0 || !boData.data) {
        return false;
    }

    if (!m_codecContext) {
        return false;
    }

    int ret = avcodec_send_packet(m_codecContext, (AVPacket *)boData.data);
    if (ret != 0) {
        PRINT_FFMPEG_ERROR(ret);
        return false;
    }
    return true;
}

BoData FFDecoder::recvFrame() {
    if (!m_codecContext) {
        return BoData();
    }

    if (!m_frame) {
        m_frame = av_frame_alloc();
    }

    //再次调用会复用上次空间，线程不安全
    int ret = avcodec_receive_frame(m_codecContext, m_frame);
    if (ret != 0) {
        // PRINT_FFMPEG_ERROR(ret);
        return BoData();
    }

    BoData boData;
    boData.data = (unsigned char *)m_frame;

    if (AVMEDIA_TYPE_VIDEO == m_codecContext->codec_type) {
        // yuv
        boData.size = (m_frame->linesize[0] + m_frame->linesize[1] +
                       m_frame->linesize[2]) *
                      m_frame->height;

        boData.width = m_frame->width;
        boData.height = m_frame->height;

    } else if (AVMEDIA_TYPE_AUDIO == m_codecContext->codec_type) {
        //样本字节数 * 单通道样本数 * 通道数
        boData.size = av_get_bytes_per_sample((AVSampleFormat)m_frame->format) *
                      m_frame->nb_samples * m_frame->ch_layout.nb_channels;
    }
    memcpy(boData.datas, m_frame->data, sizeof(boData.datas));
    return boData;
}

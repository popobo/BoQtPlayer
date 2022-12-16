#include "QAudioPlayer.h"
#include "BoLog.h"

#include <QAudioFormat>
#include <QMediaDevices>
extern "C" {
#include "libswresample/swresample.h"
}

class AudioBuffer : public QIODevice {
  public:
    qint64 readData(char *data, qint64 len) override {
        // 这边不阻塞好像会有问题
        BO_INFO("AudioBuffer readData");
        std::unique_lock<std::mutex> locker;
        if (len >= m_sizeOfUsed) {
            int readLen = m_sizeOfUsed;
            memcpy(data, m_buffer, m_sizeOfUsed);
            memset(m_buffer, 0, BUFFER_LEN);
            m_sizeOfUsed = 0;
            return readLen;
        } else {
            int readLen = len;
            memcpy(data, m_buffer, len);
            memset(data, 0, len);
            m_sizeOfUsed = m_sizeOfUsed - len;
            return readLen;
        }
    }

    qint64 writeData(const char *data, qint64 len) override {
        BO_INFO("AudioBuffer writeData");
        std::unique_lock<std::mutex> locker;
        len =
            len > (BUFFER_LEN - m_sizeOfUsed) ? BUFFER_LEN - m_sizeOfUsed : len;

        memcpy(m_buffer + m_sizeOfUsed, data, len);
        m_sizeOfUsed += len;
        return len;
    }

    qint64 bytesToWrite() const override {
        std::unique_lock<std::mutex> locker;
        return BUFFER_LEN - m_sizeOfUsed;
    }

    static const qint64 BUFFER_LEN = 1024 * 20;

  private:
    char m_buffer[BUFFER_LEN]{0};
    qint64 m_sizeOfUsed{0};
    std::mutex m_bufferMutex;
};

QAudioPlayer::QAudioPlayer() {}

QAudioPlayer::~QAudioPlayer() {
    m_audioBuffer->close();
    m_audioSink->stop();
}

bool QAudioPlayer::open() {
    if (m_isStarted) {
        BO_INFO("QAudioPlayer has been start");
        return false;
    }

    if (!m_audioBuffer) {
        m_audioBuffer = std::make_shared<AudioBuffer>();
    }

    m_audioDevice = QMediaDevices::defaultAudioOutput();
    m_qPreferedAudioFormat = m_audioDevice.preferredFormat();
    switch (m_qPreferedAudioFormat.sampleFormat()) {
    case QAudioFormat::SampleFormat::UInt8:
        m_audioOutFormat.sampleBits = SampleBits::UInt8;
        break;
    case QAudioFormat::SampleFormat::Int16:
        m_audioOutFormat.sampleBits = SampleBits::Int16;
        break;
    case QAudioFormat::SampleFormat::Int32:
        m_audioOutFormat.sampleBits = SampleBits::Int32;
        break;
    case QAudioFormat::SampleFormat::Float:
        m_audioOutFormat.sampleBits = SampleBits::Float;
        break;
    default:
        break;
    }
    m_audioOutFormat.sampleChannelCount = m_qPreferedAudioFormat.channelCount();
    m_audioOutFormat.sampleRate = m_qPreferedAudioFormat.sampleRate();

    m_audioBuffer->open(QIODevice::ReadWrite);

    if (!m_audioSink) {
        m_audioSink =
            std::make_shared<QAudioSink>(m_audioDevice, m_qPreferedAudioFormat);
    }

    m_isStarted = true;
    return true;
}

void QAudioPlayer::update(const std::shared_ptr<IBoData> &boData) {
    if (!boData || boData->size() <= 0 || !boData->data()) {
        return;
    }

    while (!m_isExit) {
        if (m_audioBuffer->bytesToWrite() >= boData->size()) {
            m_audioBuffer->writeData((char *)boData->data(), boData->size());
            break;
        }

        boSleep(1);
    }
}

bool QAudioPlayer::start() {
    // 注意这边的生命周期
    if (!m_audioSink) {
        return false;
    }
    m_audioSink->start(m_audioBuffer.get());
    return true;
}

void QAudioPlayer::stop() { m_isExit = true; }

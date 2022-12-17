#include "QAudioPlayer.h"
#include "BoLog.h"

#include <QAudioFormat>
#include <QMediaDevices>
#include <QThread>
#include <queue>
extern "C" {
#include "libswresample/swresample.h"
}

class AudioBuffer : public QIODevice {
  public:
    qint64 readData(char *data, qint64 len) override {
        // 一般len远大于boData->size()
        if (m_boDataQueue.empty()) {
            return 0;
        }
        qint64 readLen = 0;

        while (!m_boDataQueue.empty()) {
            auto boData = m_boDataQueue.front();
            if (readLen + boData->size() > len) {
                break;
            }
            memcpy(data + readLen, boData->data(), boData->size());
            readLen += boData->size();
            m_boDataQueue.pop();
        }

        return readLen;
    }

    void addBoData(const std::shared_ptr<IBoData> &newBoData) {
        m_boDataQueue.push(newBoData);
    }

    qint64 writeData(const char *data, qint64 len) override { return 0; }

  private:
    std::queue<std::shared_ptr<IBoData>> m_boDataQueue;
};

QAudioPlayer::QAudioPlayer() {}

QAudioPlayer::~QAudioPlayer() {}

bool QAudioPlayer::open(const std::shared_ptr<IParameter> &parameter) {
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
    m_timeBase = parameter->timeBase();

    m_isStarted = true;
    return true;
}

void QAudioPlayer::update(const std::shared_ptr<IBoData> &boData) {
    if (!boData || boData->size() <= 0 || !boData->data()) {
        return;
    }
    if (!m_isExit) {
        m_audioBuffer->addBoData(boData);
    }
}

bool QAudioPlayer::start() {
    // 注意这边的生命周期
    if (!m_audioSink) {
        return false;
    }
    m_audioSink->start(m_audioBuffer.get());
    BoThread::start();
    return true;
}

void QAudioPlayer::stop() {
    m_audioSink->stop();
    m_audioBuffer->close();
    BoThread::stop();
    m_isExit = true;
}

std::shared_ptr<IBoData> QAudioPlayer::getData() { return nullptr; }

void QAudioPlayer::main() {
    while (!m_isExit) {
        m_pts = m_audioSink->processedUSecs() / 1000;
        // BO_INFO("m_pts: {0}", m_pts);
        boSleep(1);
    }
}

#include "QAudioPlayer.h"
#include "BoLog.h"
#include <QAudioFormat>
#include <QMediaDevices>
#include <QThread>
#include <mutex>
#include <queue>
extern "C" {
#include "libswresample/swresample.h"
}
#include "bo_thread_safe_queue.h"

class AudioBuffer : public QIODevice {
  public:
    qint64 readData(char *data, qint64 len) override {
        // 一般len远大于boData->size()
        qint64 readLen = 0;

        while (!m_boData_queue.empty()) {
            auto boData = m_boData_queue.front();

            if (readLen + boData->size() > len) {
                break;
            }

            m_boData_queue.pop();

            memcpy(data + readLen, boData->data(), boData->size());
            readLen += boData->size();
        }

        return readLen;
    }

    void addBoData(std::shared_ptr<IBoData> &newBoData) {
        m_boData_queue.push(newBoData);
    }

    qint64 writeData(const char *data, qint64 len) override { return 0; }

    void clear() { m_boData_queue.clean(); }

  private:
    bo_thread_safe_queue<std::shared_ptr<IBoData>> m_boData_queue{};
};

QAudioPlayer::QAudioPlayer() {
    m_audioPlayerThread = new QThread;
    // 别忘记start
    m_audioPlayerThread->start();
    this->moveToThread(m_audioPlayerThread);
    connect(m_audioPlayerThread, &QThread::finished, m_audioPlayerThread,
            &QObject::deleteLater);
    connect(this, &QAudioPlayer::signalOpen, this, &QAudioPlayer::slotOpen);
    connect(this, &QAudioPlayer::signalStart, this, &QAudioPlayer::slotStart);
    connect(this, &QAudioPlayer::signalStop, this, &QAudioPlayer::slotStop);
    connect(this, &QAudioPlayer::signalPause, this, &QAudioPlayer::slotPause);
    connect(this, &QAudioPlayer::signalResume, this, &QAudioPlayer::slotResume);
}

QAudioPlayer::~QAudioPlayer() {}

bool QAudioPlayer::open(const std::shared_ptr<IParameter> &parameter) {
    if (m_isStarted) {
        BO_INFO("QAudioPlayer has been start");
        return false;
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

    emit signalOpen();

    return true;
}

void QAudioPlayer::update(std::shared_ptr<IBoData> boData) {
    if (!boData || boData->size() <= 0 || !boData->data() || !m_audioBuffer) {
        return;
    }

    if (!m_isExit) {
        m_audioBuffer->addBoData(boData);
    }
}

bool QAudioPlayer::start() {
    emit signalStart();
    return true;
}

void QAudioPlayer::stop() { emit signalStop(); }

void QAudioPlayer::setBasePts(int64_t newPts) {
    if (!m_audioSink) {
        return;
    }

    m_basePts = newPts;
    m_fixPts = -m_audioSink->processedUSecs() / 1000;
    m_pts = newPts;
}

namespace {
qint64 lastProcessedUSecs = 0;
qint64 currentProcessedUSecs = 0;
} // namespace

long QAudioPlayer::getPts() {
    // 单位ms
    if (!m_audioSink) {
        return 0;
    }

    currentProcessedUSecs = m_audioSink->processedUSecs() / 1000;
    if (m_isPaused) {
        return currentProcessedUSecs;
    }

    if (currentProcessedUSecs != lastProcessedUSecs) {
        m_timer.elapsed();
        m_pts = currentProcessedUSecs + m_basePts + m_fixPts;
        lastProcessedUSecs = currentProcessedUSecs;
    } else {
        m_pts += m_timer.elapsed();
    }

    return m_pts;
}

std::shared_ptr<IBoData> QAudioPlayer::getData() { return nullptr; }

void QAudioPlayer::pause() { emit signalPause(); }

bool QAudioPlayer::isPaused() { return m_isPaused; }

void QAudioPlayer::resume() { emit signalResume(); }

void QAudioPlayer::clear() { m_audioBuffer->clear(); }

const AudioOutputFormat &QAudioPlayer::audioOutFormat() const {
    return m_audioOutFormat;
}

void QAudioPlayer::slotOpen() {
    if (!m_audioBuffer) {
        m_audioBuffer = std::make_shared<AudioBuffer>();
    }

    m_audioBuffer->open(QIODevice::ReadWrite);

    if (!m_audioSink) {
        m_audioSink =
            std::make_shared<QAudioSink>(m_audioDevice, m_qPreferedAudioFormat);
    }

    m_isStarted = true;
}

void QAudioPlayer::slotStart() {
    // 注意这边的生命周期
    if (!m_audioSink) {
        return;
    }
    m_audioSink->start(m_audioBuffer.get());
    m_isPaused = false;
    return;
}

void QAudioPlayer::slotStop() {
    if (!m_audioSink || !m_audioBuffer || !m_audioPlayerThread) {
        return;
    }

    m_audioSink->stop();
    m_audioBuffer->close();
    m_audioPlayerThread->quit();
    m_isExit = true;
}

void QAudioPlayer::slotPause() {
    if (!m_audioSink) {
        return;
    }

    m_isPaused = true;
    m_audioSink->suspend();
}

void QAudioPlayer::slotResume() {
    if (!m_audioSink) {
        return;
    }

    m_isPaused = false;
    m_audioSink->resume();
}

#include "IFrameDispatcher.h"
#include "BoLog.h"

IFrameDispatcher::IFrameDispatcher() {}

bool IFrameDispatcher::open(std::shared_ptr<IParameter> audioPara,
                            std::shared_ptr<IParameter> videoPara) {
    m_audioPara = audioPara;
    m_videoPara = videoPara;
    return true;
}

void IFrameDispatcher::update(const std::shared_ptr<IBoData> &boData) {
    if (!boData) {
        return;
    }

    if (boData->isAudio()) {
        BO_INFO("boData->pts():{0}", boData->pts());
        m_audioFrames.push(boData);
    } else {
        m_videoFrames.push(boData);
    }
}

void IFrameDispatcher::main() {
    while (!m_isExit) {
        if (!m_audioFrames.empty()) {
            auto audioFrame = m_audioFrames.front();
            m_currentAudioPts = audioFrame->pts() * m_audioPara->timeBase();
            m_audioFrames.pop();
        }

        if (!m_videoFrames.empty()) {
            auto videoFrame = m_audioFrames.front();
            m_currentVideoPts = videoFrame->pts() * m_videoPara->timeBase();
            m_videoFrames.pop();
        }

        BO_INFO("m_currentAudioPts: {0}, m_currentVideoPts: {1}",
                m_currentAudioPts, m_currentVideoPts);

        boSleep(1);
    }
}

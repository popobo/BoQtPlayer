#include "BoData.h"
#include "BoLog.h"
extern "C" {
#include "libavformat/avformat.h"
}

unsigned char *BoData::getData() { return m_data; }

void BoData::drop() {
    if (!m_data) {
        return;
    }
    av_packet_free((AVPacket **)&m_data);
    m_data = nullptr;
    m_size = 0;
}

void BoData::setData(unsigned char *newData) { m_data = newData; }

void BoData::setSize(int newSize) { m_size = newSize; }

BoData::BoData() {}

BoData::BoData(const BoData &boData) {
    BO_INFO("test");
    m_data = boData.m_data;
    m_size = boData.m_size;
}

BoData &BoData::operator=(const BoData &boData) {
    BO_INFO("test");
    m_data = boData.m_data;
    m_size = boData.m_size;
    return *this;
}

bool BoData::isAudio() const { return m_isAudio; }

void BoData::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

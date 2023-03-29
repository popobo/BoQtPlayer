#include "BoAudioData.h"
#include <cstring>

BoAudioData::BoAudioData() {}

BoAudioData::~BoAudioData() {
    if (m_data) {
        delete[] m_data;
        m_data = nullptr;
    }
}

BoAudioData::BoAudioData(const BoAudioData &dataIn) {
    if (!dataIn.m_data) {
        return;
    }

    m_data = new uint8_t[dataIn.m_size];
    m_size = dataIn.m_size;
}

BoAudioData &BoAudioData::operator=(const BoAudioData &dataIn) {
    this->drop();

    if (!dataIn.m_data) {
        return *this;
    }

    m_data = new uint8_t[dataIn.m_size];
    m_size = dataIn.m_size;

    return *this;
}

bool BoAudioData::alloc(int size, const uint8_t *dataIn) {
    if (size <= 0) {
        return false;
    }

    m_data = new uint8_t[size];
    if (m_data) {
        m_size = size;
        if (dataIn) {
            memcpy(m_data, dataIn, m_size);
        }
    }

    return true;
}

void BoAudioData::drop() {
    if (m_data) {
        delete[] m_data;
        m_data = nullptr;
    }
    m_size = 0;
}

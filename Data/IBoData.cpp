#include "IBoData.h"

IBoData::IBoData() {}

IBoData::~IBoData() {}

bool IBoData::alloc(int size, const uint8_t *dataIn) { return false; }

void IBoData::drop() {}

void *IBoData::structDataPtr() const { return m_structDataPtr; }

void IBoData::setStructDataPtr(void *newStructDataPtr) {
    m_structDataPtr = newStructDataPtr;
}

uint8_t *IBoData::data() const { return m_data; }

void IBoData::setData(uint8_t *newData) { m_data = newData; }

std::vector<uint8_t *> IBoData::datas() const { return m_datas; }

void IBoData::addDatas(uint8_t *data) { m_datas.push_back(data); }

void IBoData::setDatas(const std::vector<uint8_t *> &datas) { m_datas = datas; }

int IBoData::width() const { return m_width; }

void IBoData::setWidth(int newWidth) { m_width = newWidth; }

int IBoData::height() const { return m_height; }

void IBoData::setHeight(int newHeight) { m_height = newHeight; }

int IBoData::format() const { return m_format; }

void IBoData::setFormat(int newFormat) { m_format = newFormat; }

bool IBoData::isAudio() const { return m_isAudio; }

void IBoData::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

int IBoData::size() const { return m_size; }

void IBoData::setSize(int newSize) { m_size = newSize; }

void IBoData::copyBasicAttributes(const IBoData &boData) {
    this->m_size = boData.m_size;
    this->m_width = boData.m_width;
    this->m_height = boData.m_height;
    this->m_format = boData.m_format;
    this->m_isAudio = boData.m_isAudio;
    this->m_pts = boData.m_pts;
}

int64_t IBoData::pts() const { return m_pts; }

void IBoData::setPts(int64_t newPts) { m_pts = newPts; }

double IBoData::timeBase() const { return m_timeBase; }

void IBoData::setTimeBase(double newTimeBase) { m_timeBase = newTimeBase; }

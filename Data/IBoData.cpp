#include "IBoData.h"

IBoData::IBoData() {}

IBoData::~IBoData() {}

void *IBoData::structDataPtr() const { return m_structDataPtr; }

void IBoData::setStructDataPtr(void *newStructDataPtr) {
    m_structDataPtr = newStructDataPtr;
}

unsigned char *IBoData::data() const { return m_data; }

void IBoData::setData(unsigned char *newData) { m_data = newData; }

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
}

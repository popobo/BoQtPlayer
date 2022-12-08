#include "IDecoder.h"

bool IDecoder::isAudio() const { return m_isAudio; }

void IDecoder::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

void IDecoder::main() {
    while (!m_isExit) {
        std::unique_lock<std::mutex> lock(m_boDataListMutex);
        if (m_boDataList.empty()) {
            lock.unlock();
            boSleep(1);
            continue;
        }

        //取出packet 消费者
        BoData boData = m_boDataList.front();
        m_boDataList.pop_front();

        //开启解码
        //发送数据到解码线程 一个数据包可能解码多个结果(主要是音频)
        if (sendPacket(boData)) {
            while (!m_isExit) {
                //获取解码器
                //获取解码数据
                BoData frame = recvFrame();
                if (!frame.data) {
                    break;
                }
                //发送数据给观察者
                notify(frame);
            }
        }
        //消费者负责清理
        boData.drop();
    }
}

void IDecoder::update(BoData boData) {
    if (boData.isAudio != m_isAudio) {
        return;
    }
    // why循环
    while (!m_isExit) {
        std::unique_lock<std::mutex> lock(m_boDataListMutex);
        if (m_boDataList.size() < MAX_LIST) {
            m_boDataList.push_back(boData);
            break;
        }
        lock.unlock();
        boSleep(1);
    }
}

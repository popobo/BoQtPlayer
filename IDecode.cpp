#include "IDecode.h"

bool IDecode::isAudio() const { return m_isAudio; }

void IDecode::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

void IDecode::main() {
    while (!m_isExit) {
        std::unique_lock<std::mutex> lock(m_boDataListMutex);
        if (m_boDataList.empty()) {
            lock.unlock();
            boSleep(1);
            continue;
        }

        //去除packet 消费者
        BoData boData = m_boDataList.front();
        m_boDataList.pop_front();

        //开启解码
        //发送数据到解码线程 一个数据包可能解码多个结果(主要是音频)
        if (sendPacket(boData)) {
            while (!m_isExit) {
                //获取解码器
                //获取解码数据
                BoData frame = recvFrame();
                if (!frame.getData()) {
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

void IDecode::update(BoData boData) {
    if (boData.isAudio() != m_isAudio) {
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

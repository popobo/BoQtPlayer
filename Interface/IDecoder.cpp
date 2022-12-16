#include "IDecoder.h"
#include "BoLog.h"

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
        if (m_boDataList.empty()) {
            lock.unlock();
            boSleep(1);
            continue;
        }
        std::shared_ptr<IBoData> boData = m_boDataList.front();
        m_boDataList.pop_front();

        //开启解码
        //发送数据到解码线程 一个数据包可能解码多个结果(主要是音频)
        if (sendPacket(boData)) {
            while (!m_isExit) {
                //获取解码器
                //获取解码数据
                auto frame = recvFrame();
                if (!frame) {
                    break;
                }
                if (!m_isAudio) {
                    BO_INFO("video notify(frame)");
                }
                //发送数据给观察者
                notify(frame);
            }
        }
        //消费者负责清理
    }
}

void IDecoder::update(const std::shared_ptr<IBoData> &boData) {
    if (boData->isAudio() != m_isAudio) {
        return;
    }
    // why循环
    // 循环是为了阻塞住FFDemux让其不要读取数据了
    while (!m_isExit) {
        std::unique_lock<std::mutex> lock(m_boDataListMutex);
        if (m_boDataList.size() < MAX_LIST) {
            m_boDataList.push_back(boData);
            break;
        }
        BO_INFO(m_isAudio ? "audio" : "video", "block");
        lock.unlock();
        boSleep(1);
    }
}

#include "IDecoder.h"
#include "BoLog.h"

void IDecoder::clear()
{
    std::unique_lock<std::mutex> locker{ m_boDataQueueMutex };
    m_boDataQueue = {};
}

bool IDecoder::isAudio() const { return m_isAudio; }

void IDecoder::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

void IDecoder::main() {
    while (!m_isExit) {
        if (m_isPaused) {
            boSleep(1);
            continue;
        }

        std::unique_lock<std::mutex> lock(m_boDataQueueMutex);

        if (m_boDataQueue.empty()) {
            lock.unlock();
            boSleep(1);
            continue;
        }

        //取出packet 消费者
        if (m_boDataQueue.empty()) {
            lock.unlock();
            boSleep(1);
            continue;
        }
        std::shared_ptr<IBoData> boData = m_boDataQueue.front();
        m_boDataQueue.pop();

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

                //发送数据给观察者
                notify(frame);
            }
        }
        //消费者负责清理, 采用智能指针管理
    }
}

void IDecoder::update(const std::shared_ptr<IBoData> &boData) {
    if (boData->isAudio() != m_isAudio) {
        return;
    }

    // 循环是为了阻塞住FFDemux让其不要读取数据了
    while (!m_isExit) {
        std::unique_lock<std::mutex> lock(m_boDataQueueMutex);
        if (m_boDataQueue.size() < MAX_LIST) {
            m_boDataQueue.push(boData);
            break;
        }
        lock.unlock();
        boSleep(1);
    }
}

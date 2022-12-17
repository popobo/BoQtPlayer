#include "IDecoder.h"
#include "BoLog.h"

bool IDecoder::isAudio() const { return m_isAudio; }

void IDecoder::setIsAudio(bool newIsAudio) { m_isAudio = newIsAudio; }

int IDecoder::synPts() const { return m_synPts; }

void IDecoder::setSynPts(int newSynPts) {
    m_synPts = newSynPts;
    //    BO_INFO("m_isAudio:{0}, m_synPts:{1}, newSynPts:{2}", m_isAudio,
    //    m_synPts,
    //            newSynPts);
}

int IDecoder::pts() const { return m_pts; }

void IDecoder::setPts(int newPts) { m_pts = newPts; }

void IDecoder::main() {
    while (!m_isExit) {
        std::unique_lock<std::mutex> lock(m_boDataListMutex);
        // 判断音视频同步
        if (!m_isAudio && m_synPts > 0) {
            //当音频时间小于视频时间，等音频
            // BO_INFO("m_synPts:{1}, m_pts{1}", m_synPts, m_pts);
            if (m_synPts < m_pts) {
                lock.unlock();
                boSleep(1);
                continue;
            }
        }

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
                    m_pts = frame->pts() * m_videoTimeBase;
                    // BO_INFO("m_pts: {0}", m_pts);
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
        lock.unlock();
        boSleep(1);
    }
}

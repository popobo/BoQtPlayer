#include "IDemux.h"
#include "BoLog.h"

IDemux::~IDemux() {}

void IDemux::main() {
    while (!m_isExit) {
        auto boData = read();
        if (boData->size() > 0) {
            //通知观察者, 如果没有观察者接受数据, 数据应该销毁
            BO_INFO("notify(boData)");
            notify(boData);
        }

        if (boData->size() <= 0) {
            BO_INFO("boData->size() <= 0");
            break;
        }
    }
}

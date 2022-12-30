#include "IDemux.h"

IDemux::~IDemux() {}

void IDemux::main() {
    if (m_isPaused) {
        boSleep(1);
        return;
    }

    auto boData = read();
    if (boData->size() > 0) {
        //通知观察者, 如果没有观察者接受数据, 数据应该销毁
        notify(boData);
    }
    else {
        boSleep(1);
    }
}

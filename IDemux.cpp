#include "IDemux.h"

void IDemux::main() {
    while (!m_isExit) {
        auto boData = read();
        if (boData->size() > 0) {
            //通知观察者, 如果没有观察者接受数据, 数据应该销毁
            notify(boData);
        }

        if (boData->size() <= 0) {
            break;
        }
    }
}

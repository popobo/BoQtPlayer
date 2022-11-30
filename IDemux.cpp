#include "IDemux.h"
#include "BoLog.h"

void IDemux::main() {
    while (!m_isExit) {
        BoData boData = read();
        if (boData.getSize() > 0) {
            //通知观察者, 如果没有观察者接受数据, 数据应该销毁
            notify(boData);
        }

        if (boData.getSize() <= 0) {
            break;
        }
    }
}

#include "IDemux.h"
#include "BoLog.h"

void IDemux::main() {
    while (!m_isExit) {
        BoData boData = read();
        if (boData.getSize() > 0) {
            notify(boData);
        }
        BO_INFO("IDemux read {0}", boData.getSize());
        if (boData.getSize() <= 0) {
            break;
        }
    }
}

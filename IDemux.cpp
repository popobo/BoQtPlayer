#include "IDemux.h"
#include "BoLog.h"

void IDemux::main() {
    while (!m_isExit) {
        BoData boData = read();
        BO_INFO("IDemux read {0}", boData.size);
        if (boData.size <= 0) {
            break;
        }
    }
}

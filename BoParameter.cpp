#include "BoParameter.h"

BoParameter::BoParameter() {}

const AVCodecParameters *BoParameter::getPara() const { return para; }

void BoParameter::setPara(AVCodecParameters *newPara) { para = newPara; }

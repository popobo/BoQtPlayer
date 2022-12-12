#include "BoParameter.h"

BoParameter::BoParameter() {}

AVCodecParameters *BoParameter::getPara() const { return para; }

void BoParameter::setPara(AVCodecParameters *newPara) { para = newPara; }

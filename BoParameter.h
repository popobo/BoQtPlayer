#ifndef BOPARAMETER_H
#define BOPARAMETER_H

#include <memory>
extern "C" {
#include "libavcodec/avcodec.h"
}
class BoParameter {
  public:
    BoParameter();

    const AVCodecParameters *getPara() const;
    void setPara(AVCodecParameters *newPara);

  private:
    AVCodecParameters *para = nullptr;
};

#endif // BOPARAMETER_H

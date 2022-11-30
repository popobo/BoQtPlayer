#ifndef BOPARAMETER_H
#define BOPARAMETER_H

#include <memory>
extern "C" {
#include "libavcodec/avcodec.h"
}
class BoParameter {
  public:
    BoParameter();

  private:
    std::shared_ptr<AVCodecParameters> para = nullptr;
};

#endif // BOPARAMETER_H

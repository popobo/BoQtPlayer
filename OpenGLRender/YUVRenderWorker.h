#ifndef YUVRENDERWORKER_H
#define YUVRENDERWORKER_H

#include "IRenderWorker.h"
#include <QObject>

class YUVRenderWorker : public QObject, public IRenderWorker {
    Q_OBJECT

  public:
    YUVRenderWorker();
};

#endif // YUVRENDERWORKER_H

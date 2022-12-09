#ifndef FRAMEDISPATCHER_H
#define FRAMEDISPATCHER_H

#include "IObserver.h"
#include <QObject>

class FrameDispatcher : public QObject, public IObserver {
    Q_OBJECT

  public:
    FrameDispatcher();

    // IObserver interface
    virtual void update(const std::shared_ptr<BoData> &boData) override;

  signals:
    void sendData(const std::shared_ptr<BoData> &boData);
};

#endif // FRAMEDISPATCHER_H

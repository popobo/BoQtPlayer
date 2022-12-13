#ifndef FFQTFRAMEDISPATHCER_H
#define FFQTFRAMEDISPATHCER_H

#include "IFrameDispatcher.h"

#include <QObject>

// 必须继承QObject才能使用Q_OBJECT
class FFQtFrameDispathcer : public QObject, public IFrameDispatcher {
    Q_OBJECT

  public:
    FFQtFrameDispathcer();

    // IObserver interface
    virtual void update(const std::shared_ptr<IBoData> &boData) override;

  signals:
    void sendData(const std::shared_ptr<IBoData> &boData);
};

#endif // FFQTFRAMEDISPATHCER_H

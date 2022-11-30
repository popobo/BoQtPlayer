#ifndef ISUBJECT_H
#define ISUBJECT_H

#include "BoData.h"
#include "IObserver.h"
#include <memory>
#include <mutex>
#include <vector>

class ISubject {
  public:
    virtual void addObs(std::weak_ptr<IObserver> observer);

    virtual void delObs(std::weak_ptr<IObserver> delObserver);

    void notify(BoData boData);

  protected:
    std::vector<std::weak_ptr<IObserver>> m_observers;
    std::mutex m_mux;
};

#endif // ISUBJECT_H

#pragma once

#include "Data/IBoData.h"
#include "IObserver.h"
#include <list>
#include <memory>
#include <mutex>

class ISubject {
  public:
    ISubject() {}

    virtual ~ISubject() {}

    virtual void addObs(std::weak_ptr<IObserver> observer);

    virtual void addStrongObs(std::shared_ptr<IObserver> observer);

    virtual void delObs(std::weak_ptr<IObserver> delObserver);

    virtual void delStrongObs(std::shared_ptr<IObserver> observer);

    virtual void notify(std::shared_ptr<IBoData> boData);

    virtual bool isAnyObserverSatisfied();

  protected:
    std::list<std::weak_ptr<IObserver>> m_observers;
    std::list<std::shared_ptr<IObserver>> m_strongObservers;
    std::mutex m_mux;
};

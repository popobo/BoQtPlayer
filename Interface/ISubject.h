#ifndef ISUBJECT_H
#define ISUBJECT_H

#include "Data/IBoData.h"
#include "IObserver.h"
#include <memory>
#include <mutex>
#include <vector>

class ISubject {
  public:
    ISubject() {}

    virtual ~ISubject() {}

    virtual void addObs(const std::weak_ptr<IObserver> &observer);

    virtual void addStrongObs(const std::shared_ptr<IObserver> &observer);

    virtual void delObs(const std::weak_ptr<IObserver> &delObserver);

    virtual void delStrongObs(const std::shared_ptr<IObserver> &observer);

    virtual void notify(const std::shared_ptr<IBoData> &boData);

    virtual bool isAnyObserverSatisfied();

  protected:
    std::vector<std::weak_ptr<IObserver>> m_observers;
    std::vector<std::shared_ptr<IObserver>> m_strongObservers;
    std::mutex m_mux;
};

#endif // ISUBJECT_H

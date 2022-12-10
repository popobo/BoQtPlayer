#include "ISubject.h"

void ISubject::addObs(const std::weak_ptr<IObserver> &observer) {
    if (!observer.lock()) {
        return;
    }
    std::unique_lock<std::mutex> lock(m_mux);
    m_observers.push_back(observer);
}

void ISubject::addStrongObs(const std::shared_ptr<IObserver> &observer) {
    m_strongObservers.push_back(observer);
}

void ISubject::delObs(const std::weak_ptr<IObserver> &delObserver) {
    auto delIterator =
        std::find_if(m_observers.begin(), m_observers.end(),
                     [&delObserver](const std::weak_ptr<IObserver> &observer) {
                         return delObserver.lock() == observer.lock();
                     });
    m_observers.erase(delIterator);
}

void ISubject::delStrongObs(const std::shared_ptr<IObserver> &delObserver) {
    auto delIterator = std::find_if(
        m_strongObservers.begin(), m_strongObservers.end(),
        [&delObserver](const std::shared_ptr<IObserver> &observer) {
            return delObserver == observer;
        });
    m_strongObservers.erase(delIterator);
}

void ISubject::notify(const std::shared_ptr<IBoData> &boData) {
    std::unique_lock<std::mutex> lock(m_mux);
    // 小心少了引用
    for (auto &observer : m_observers) {
        if (auto observerPtr = observer.lock()) {
            observerPtr->update(boData);
        }
    }

    for (auto &observer : m_strongObservers) {
        observer->update(boData);
    }
}

#include "ISubject.h"

void ISubject::addObs(std::weak_ptr<IObserver> observer) {
    if (!observer.lock()) {
        return;
    }
    std::unique_lock<std::mutex> lock(m_mux);
    m_observers.push_back(observer);
}

void ISubject::delObs(std::weak_ptr<IObserver> delObserver) {
    auto delIterator =
        std::find_if(m_observers.begin(), m_observers.end(),
                     [&delObserver](const std::weak_ptr<IObserver> observer) {
                         return delObserver.lock() == observer.lock();
                     });
    m_observers.erase(delIterator);
}

void ISubject::notify(BoData boData) {
    std::unique_lock<std::mutex> lock(m_mux);
    // 小心少了引用
    for (auto &observer : m_observers) {
        if (auto observerPtr = observer.lock()) {
            observerPtr->update(boData);
        }
    }
}

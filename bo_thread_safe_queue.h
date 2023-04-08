#pragma once

#include <condition_variable>
#include <limits>
#include <memory>
#include <mutex>
#include <queue>

template <typename T> class bo_thread_safe_queue {
  private:
    mutable std::mutex m_mut;
    std::condition_variable m_cv;
    std::queue<T> m_queue;
    std::size_t m_max_size;

  public:
    bo_thread_safe_queue(
        std::size_t max_size = std::numeric_limits<std::size_t>::max())
        : m_max_size(max_size) {}
    bo_thread_safe_queue(const bo_thread_safe_queue &) = delete;
    bo_thread_safe_queue &operator=(const bo_thread_safe_queue &) = delete;
    bo_thread_safe_queue(const bo_thread_safe_queue &&) = delete;
    bo_thread_safe_queue &operator=(const bo_thread_safe_queue &&) = delete;
    ~bo_thread_safe_queue() = default;

    static const int32_t DEFAULT_WAIT_TIME = 100;

    T &front() {
        std::lock_guard ul{m_mut};
        return m_queue.front();
    }

    void pop() {
        std::lock_guard lg{m_mut};
        m_queue.pop();
        m_cv.notify_one();
    }

    void push(const T &value) {
        std::lock_guard lg{m_mut};
        m_queue.push(value);
        m_cv.notify_one();
    }

    void push_for(const T &value, int32_t ms = DEFAULT_WAIT_TIME) {
        std::unique_lock ul{m_mut};
        if (!m_cv.wait_for(ul, std::chrono::milliseconds(ms),
                           [this] { return m_queue.size() < m_max_size; })) {
            return;
        }

        m_queue.push(value);
        m_cv.notify_one();
    }

    bool try_pop_front(T &value) {
        std::lock_guard lg{m_mut};
        if (m_queue.empty()) {
            return false;
        }
        value = m_queue.front();
        m_queue.pop();
        m_cv.notify_one();
        return true;
    }

    std::shared_ptr<T> try_pop_front() {
        std::lock_guard lg{m_mut};
        if (m_queue.empty()) {
            return nullptr;
        }
        auto res{std::make_shared<T>(m_queue.front())};
        m_queue.pop();
        m_cv.notify_one();
        return res;
    }

    bool wait_for_pop_front(T &value, int32_t ms = DEFAULT_WAIT_TIME) {
        std::unique_lock ul{m_mut};
        bool cv_status = m_cv.wait_for(ul, std::chrono::milliseconds{ms},
                                       [this] { return !m_queue.empty(); });

        if (!cv_status) {
            return false;
        }

        value = m_queue.front();
        m_queue.pop();
        m_cv.notify_one();
        return true;
    }

    std::shared_ptr<T> wait_for_pop_front(int32_t ms = DEFAULT_WAIT_TIME) {
        std::unique_lock ul{m_mut};
        bool cv_status = m_cv.wait_for(ul, std::chrono::milliseconds{ms},
                                       [this] { return !m_queue.empty(); });

        if (!cv_status) {
            return std::make_shared();
        }

        auto res{std::make_shared<T>(m_queue.front())};
        m_queue.pop();
        m_cv.notify_one();
        return true;
    }

    bool empty() const {
        std::lock_guard lg{m_mut};
        return m_queue.empty();
    }

    void clean() {
        std::lock_guard lg{m_mut};
        m_queue = {};
    }

    std::size_t size() const {
        std::lock_guard lg{m_mut};
        return m_queue.size();
    }
};

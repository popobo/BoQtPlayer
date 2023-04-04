#pragma once

#include <condition_variable>
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
    bo_thread_safe_queue(std::size_t max_size) : m_max_size(max_size) {}
    bo_thread_safe_queue(const bo_thread_safe_queue &) = delete;
    bo_thread_safe_queue &operator=(const bo_thread_safe_queue &) = delete;
    bo_thread_safe_queue(const bo_thread_safe_queue &&) = delete;
    bo_thread_safe_queue &operator=(const bo_thread_safe_queue &&) = delete;
    ~bo_thread_safe_queue() = default;

    static const int32_t DEFAULT_WAIT_TIME = 100;

    void push(T &value, int32_t ms = DEFAULT_WAIT_TIME) {
        std::unique_lock ul{m_mut};
        m_cv.wait_for(ul, std::chrono::milliseconds(ms),
                      [this] { return m_queue.size() < m_max_size; });

        m_queue.push(value);
        m_cv.notify_one();
    }

    bool try_pop(T &value) {
        std::lock_guard lg{m_mut};
        if (m_queue.empty()) {
            return false;
        }
        value = m_queue.front();
        m_queue.pop();
        m_cv.notify_one();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard lg{m_mut};
        if (m_queue.empty()) {
            return false;
        }
        auto res{std::make_shared<T>(m_queue.front())};
        m_queue.pop();
        m_cv.notify_one();
        return res;
    }

    bool wait_for_pop(T &value, int32_t ms = DEFAULT_WAIT_TIME) {
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

    std::shared_ptr<T> wait_for_pop(int32_t ms = DEFAULT_WAIT_TIME) {
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
};

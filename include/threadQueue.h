#ifndef THREAD_QUEUE_H
#define THREAD_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadQueue
{
public:
    ThreadQueue() = default;
    ~ThreadQueue() = default;

    ThreadQueue(const ThreadQueue&) = delete;
    ThreadQueue& operator=(const ThreadQueue&) = delete;

    void Push(const T& value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(value);
        }
        m_cv.notify_one();
    }

    bool Pop(T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [this]() {
            return !m_queue.empty() || m_stopped;
        });

        if (m_queue.empty())
            return false;

        value = m_queue.front();
        m_queue.pop();
        return true;
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopped = true;
        }
        m_cv.notify_all();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_stopped = false;
};

#endif
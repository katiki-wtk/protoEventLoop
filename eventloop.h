#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <QDebug>

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

//#define USE_PRIORITY_QUEUE

/*!
 * \brief The EventLoop class
 */
class EventLoop
{
public:
    struct Compare;

    using callable_t = std::function<void()>;
    using priority_data_t = std::tuple<int, callable_t>;
#ifdef USE_PRIORITY_QUEUE
    using priority_queue_t = std::priority_queue<priority_data_t, std::vector<priority_data_t>, Compare>;
#else
    using priority_queue_t = std::vector<callable_t>;
#endif

    /*!
     * \brief The Compare struct provides a functor to test callable priority
     */
    struct Compare
    {
        bool operator()(const priority_data_t& lsh, const priority_data_t& rsh) {
            return (std::get<0>(lsh) > std::get<0>(rsh));
        }
    };

    EventLoop() = default;
    ~EventLoop() noexcept
    {
        // Push termination in the queue
        post([this] {
                    m_running = false;
        });
        m_thread.join();
    }

    // Forbid copy/move ctor and =
    EventLoop(const EventLoop&) = delete;
    EventLoop(EventLoop&&) noexcept = delete;
    EventLoop& operator= (const EventLoop&) = delete;
    EventLoop& operator= (EventLoop&&) noexcept = delete;


    /*!
     * \brief Posts a generic \a callable with a given \a priority
     */
    void post(callable_t&& callable, const bool priority=false) noexcept
    {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
#ifdef USE_PRIORITY_QUEUE
            m_writeQueue.emplace(priority, std::move(callable));
#else
            if (priority) {
                m_writeQueue.emplace(m_writeQueue.begin(), std::move(callable));
            } else {
                m_writeQueue.emplace_back(std::move(callable));
            }
#endif
        }
        m_condition.notify_one();
    }


    /*!
     * \brief Sends a \a callable in synchronous way
     * The callable is put on top of the FIFO list
     * The caller is blocked until the callable is consumed and its result eventually returned
     */
    template<typename Func, typename ...Args>
    auto send(Func&& callable, Args&& ...args) {
        if (std::this_thread::get_id() == m_thread.get_id())
        {
            return std::invoke(
                std::forward<Func>(callable),
                std::forward<Args>(args)...);
        }

        using return_type = std::invoke_result_t<Func, Args...>;
        using packaged_task_type =
            std::packaged_task<return_type(Args&&...)>;

        packaged_task_type task(std::forward<Func>(callable));

        post([&]
                {
                    task(std::forward<Args>(args)...);
                }, true); // Posted at top priority

        return task.get_future().get();

    }


private:
    void threadFunc() noexcept
    {
        priority_queue_t readQueue;

        /*
         * While running, wait for the queue to be not empty
         * Swap the reading queue with it to work on it
         * => and let other threads pushing events in the write queue
         */
        while (m_running)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condition.wait(lock, [this] {
                    return !m_writeQueue.empty();
                });
                // Supposed to be as fast as possible
                std::swap(readQueue, m_writeQueue);
            }

            qDebug() << "process readQueue: #" << readQueue.size();

#ifdef USE_PRIORITY_QUEUE
            while (!readQueue.empty())
            {
                // Could be customized...
                auto & obj = readQueue.top();
                std::get<1>(obj)();
                readQueue.pop();
            }
#else
            for (auto& callable : readQueue)
            {
                try {
                    callable();
                }
                catch (std::exception exc) {
                    std::cerr << __FUNCTION__ << " - exception caught: " << exc.what() << std::endl;
                }
                catch (...) {
                    std::cerr << __FUNCTION__ << " - unexpected exception caught. " << std::endl;
                }
            }
#endif
        }
    }


private:
    priority_queue_t m_writeQueue;  /*! Keep in mind the priority system */
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_running {true};
    std::thread m_thread {&EventLoop::threadFunc, this};

};


#endif // EVENTLOOP_H

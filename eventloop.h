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
    using callable_t = std::function<void()>;

    EventLoop() = default;


    ~EventLoop() noexcept {
        post([this](){stop();});
        join();
    }

    // Other deleted copy/move constructors and operators...

    void post(callable_t&& callable, bool priority = false) noexcept {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_events.emplace_back(priority, std::move(callable));
        }
        m_condition.notify_one();
    }

    template<typename Func, typename ...Args>
    auto send(Func&& callable, Args&& ...args) {
        if (isInEventLoopThread()) {
            return std::invoke(std::forward<Func>(callable), std::forward<Args>(args)...);
        }

        using return_type = std::invoke_result_t<Func, Args...>;
        using packaged_task_type = std::packaged_task<return_type(Args&&...)>;

        packaged_task_type task(std::forward<Func>(callable));

        post([&] {
            task(std::forward<Args>(args)...);
        }, true);

        return task.get_future().get();
    }

    void stop() noexcept {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_running = false;
        }
        m_condition.notify_one();
    }

    void join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
    struct Event {
        bool priority;
        callable_t callable;

        Event(bool p, callable_t c) : priority(p), callable(std::move(c)) {}
    };

    std::vector<Event> m_events;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_running{true};
    std::thread m_thread{&EventLoop::threadFunc, this};

    void processEvents(const std::vector<Event>& events) noexcept {
        for (const auto& event : events) {
            try {
                event.callable();
            } catch (const std::exception& exc) {
                std::cerr << "Exception caught: " << exc.what() << std::endl;
            } catch (...) {
                std::cerr << "Unexpected exception caught." << std::endl;
            }
        }
    }

    bool isInEventLoopThread() const noexcept {
        return std::this_thread::get_id() == m_thread.get_id();
    }

    void threadFunc() noexcept {
        while (m_running) {
            std::vector<Event> events;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condition.wait(lock, [this] {
                    return !m_events.empty() || !m_running;
                });

                std::swap(events, m_events);
            }

            processEvents(events);

        }
        //std::cerr << "EXIT EVENTLOOP" << std::endl;
    }
};



#endif // EVENTLOOP_H

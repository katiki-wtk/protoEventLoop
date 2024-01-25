#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <QDebug>

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <tuple>
#include <vector>


struct TechErr
{
    int x {0};
    int y {1};
    int z {2};
};

#define FK_SYSTEM_EROR(a,b,c,m) std::cerr << "TechErr: " << a << b << c << m << std::endl

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
            if (priority) {
                m_events.emplace_front(priority, std::move(callable));
            } else {
                m_events.emplace_back(priority, std::move(callable));
            }
        }
        m_condition.notify_one();
    }

    void post(TechErr& err)
    {
        post([err]() {
            FK_SYSTEM_EROR(err.x, err.y, err.z, "TechErr raised");

        }, true);

    }

    /*!
     * \brief send
     * \param callable
     * \param args
     */
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

    /*!
     * \brief stop
     */
    void stop() noexcept {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_running = false;
        }
        m_condition.notify_one();
    }

    /*!
     * \brief join
     */
    void join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
    /*!
     * \brief The Event class
     */
    struct Event {
        bool priority;
        callable_t callable;

        Event() = default;
        Event(bool p, callable_t c) : priority(p), callable(std::move(c)) {}
    };

    /*!
     * \brief Executes the event callback
     */
    void processEvent(Event& event) {
        try {
            event.callable();
        } catch (const std::exception& exc) {
            std::cerr << "Exception caught: " << exc.what() << std::endl;
        } catch (...) {
            std::cerr << "Unexpected exception caught." << std::endl;
        }
    }

    /*!
     * \brief Threads processes event from the queue, no swap with a reading queue is done
     */
    void threadFunc() noexcept
    {
        while (m_running)
        {
            Event event;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condition.wait(lock, [this] {
                    return !m_events.empty() || !m_running;
                });

                if (!m_running)
                    break;

                event = m_events.front();
                m_events.pop_front();
            }

            processEvent(event);

        };

        qDebug() << "Exit Loop";
    }

    bool isInEventLoopThread() const noexcept {
        return std::this_thread::get_id() == m_thread.get_id();
    }


    std::deque<Event> m_events;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_running{true};
    std::thread m_thread{&EventLoop::threadFunc, this};
};


#endif // EVENTLOOP_H

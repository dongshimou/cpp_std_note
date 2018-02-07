#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class thread_pool {
public:
    explicit thread_pool(size_t capacity) noexcept {
        for (size_t i = 0; i < capacity; i++) {
            m_threads.emplace_back([=]() -> void {
                for (;;) {
                    // std::cout << "wait notify" << std::endl;
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ulock(m_lock);
                        m_notify.wait(ulock, [this] { return m_stop || !m_tasks.empty(); });
                        // std::cout << "check is return?" << std::endl;
                        if (m_stop && m_tasks.empty())
                            return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    // std::cout << "start task ....\n";
                    task();
                }
            });
        }
    }

    template<typename F, typename... Args>
    auto add(F &&f, Args &&... args) noexcept
    //* -std=c++17
    -> std::future<std::invoke_result_t<F, Args...>> {
        using r=std::invoke_result_t<F, Args...>;
        /*/// -std=c++11
        -> std::future<std::result_of_t<F(Args...)>> {
            using r = std::result_of_t<F(Args...)>;
        //*/
        auto task = std::make_shared<std::packaged_task<r()>>(
                [&]() -> r { return std::invoke(std::forward<F>(f), std::forward<Args>(args)...); });
        auto result = task->get_future();
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            if (m_stop)
                return std::async(std::launch::async, [&]() {
                    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                });
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_notify.notify_one();
        // std::cout << "start notify" << std::endl;
        return result;
    }

    ~thread_pool() {
        // std::cout << "~~~~~~~~~~" << std::endl;
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            m_stop = true;
        }
        // std::cout << "notify all stop" << std::endl;
        m_notify.notify_all();
        for (auto &t : m_threads)
            t.join();
        // std::cout << "~thread_pool" << std::endl;
    }

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_lock;
    std::condition_variable m_notify;
    bool m_stop = false;
};

//using namespace std;
//
//int main() {
//    thread_pool *pool = new thread_pool(3);
//    auto f = [](int a) {
//        cout <<"a = "<< a << endl;
//        this_thread::sleep_for(+1s);
//        return a;
//    };
//    auto res = pool->add(f, 233);
//    cout << "res = " << res.get() << endl;
//    delete pool;
//    return 0;
//}#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class thread_pool {
public:
    explicit thread_pool(size_t capacity) noexcept {
        for (size_t i = 0; i < capacity; i++) {
            m_threads.emplace_back([=]() -> void {
                for (;;) {
                    // std::cout << "wait notify" << std::endl;
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ulock(m_lock);
                        m_notify.wait(ulock, [this] { return m_stop || !m_tasks.empty(); });
                        // std::cout << "check is return?" << std::endl;
                        if (m_stop && m_tasks.empty())
                            return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    // std::cout << "start task ....\n";
                    task();
                }
            });
        }
    }

    template<typename F, typename... Args>
    auto add(F &&f, Args &&... args) noexcept
    //* -std=c++17
    -> std::future<std::invoke_result_t<F, Args...>> {
        using r=std::invoke_result_t<F, Args...>;
        /*/// -std=c++11
        -> std::future<std::result_of_t<F(Args...)>> {
            using r = std::result_of_t<F(Args...)>;
        //*/
        auto task = std::make_shared<std::packaged_task<r()>>(
                [&]() -> r { return std::invoke(std::forward<F>(f), std::forward<Args>(args)...); });
        auto result = task->get_future();
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            if (m_stop)
                return std::async(std::launch::async, [&]() {
                    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                });
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_notify.notify_one();
        // std::cout << "start notify" << std::endl;
        return result;
    }

    ~thread_pool() {
        // std::cout << "~~~~~~~~~~" << std::endl;
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            m_stop = true;
        }
        // std::cout << "notify all stop" << std::endl;
        m_notify.notify_all();
        for (auto &t : m_threads)
            t.join();
        // std::cout << "~thread_pool" << std::endl;
    }

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_lock;
    std::condition_variable m_notify;
    bool m_stop = false;
};

//using namespace std;
//
//int main() {
//    thread_pool *pool = new thread_pool(3);
//    auto f = [](int a) {
//        cout <<"a = "<< a << endl;
//        this_thread::sleep_for(+1s);
//        return a;
//    };
//    auto res = pool->add(f, 233);
//    cout << "res = " << res.get() << endl;
//    delete pool;
//    return 0;
//}


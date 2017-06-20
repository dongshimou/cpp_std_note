
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
template <typename T> class event_queue {
  public:
    event_queue(std::function<void(T)> func, size_t thread = 1) {
        for (size_t i = 0; i < thread; i++) {
            m_thread.push_back(std::move(std::thread([=]() {
                for (;;) {
                    std::cout << "----------lock" << std::endl;
                    std::unique_lock<std::mutex> ulock(m_lock);
                    m_notify.wait(ulock,
                                  [=]() { return m_stop || !m_queue.empty(); });
                    if (m_stop)
                        break;
                    auto t = std::move(m_queue.front());
                    m_queue.pop();
                    ulock.unlock();
                    std::cout << "----------unlock " << std::endl;
                    func(std::forward<T &&>(t));
                    std::cout << "thread " << i << " over" << std::endl;
                }
            })));
        }
    }
    void push(T &&t) {
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            m_queue.emplace(std::forward<T &&>(t));
        }
        m_notify.notify_one();
    }
    ~event_queue() {
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            m_stop = true;
        }
        m_notify.notify_all();
        for (auto &t : m_thread)
            t.join();
    }

  private:
    std::vector<std::thread> m_thread;
    std::queue<T> m_queue;
    std::mutex m_lock;
    std::condition_variable m_notify;
    bool m_stop{false};
};

// int main() {
//     event_queue<int> q(
//         [](int a) {
//             std::cout << "start" << std::endl;
//             for (auto j = 0; j < 100000; j++)
//                 for (auto i = 0; i < 100000; i++)
//                     ;
//             std::cout << a << std::endl;
//         },
//         4);

//     q.push(111);
//     q.push(222);
//     q.push(333);
//     q.push(444);
//     q.push(555);
//     q.push(666);
//     int temp;
//     while (std::cin >> temp) {
//         q.push(std::move(temp));
//     }
//     std::cin.get();
// }

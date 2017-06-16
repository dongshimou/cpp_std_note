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
      m_threads.emplace_back([=]() {
        for (;;) {
          // std::cout << "wait notify" << std::endl;
          std::function<void()>task;
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
  template <typename F, typename... Args>
  auto add(F &&f, Args &&... args) noexcept
      -> std::future<typename std::result_of<F(Args...)>::type> {
    using r = typename std::result_of<F(Args...)>::type;
    // std::packaged_task<r()> task([&]() -> r { return f(args...); });
    auto task = std::make_shared<std::packaged_task<r()>>(
        [&]() -> r { return f(args...); });
    auto result = task->get_future();
    {
      std::unique_lock<std::mutex> ulock(m_lock);
      if (m_stop)
        return std::async(std::launch::async,[&](){
          f(args...);
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

// using namespace std;
// int main() {
//   thread_pool *pool=new thread_pool(3);
//   cin.get();
//   auto f = [](int a) { cout << a << endl; };
//   pool->add(f, 1);
//   cin.get();
//   delete pool;
//   return 0;
// }
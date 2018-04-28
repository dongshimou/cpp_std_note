#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class thread_pool {
public:
    explicit thread_pool(size_t capacity,bool copy=false) noexcept {
        m_copy=copy;
        for (size_t i = 0; i < capacity; i++) {
            m_threads.emplace_back([=]() -> void {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ulock(m_lock);
                        m_notify.wait(ulock, [this] { return m_stop || !m_tasks.empty(); });
                        if (m_stop && m_tasks.empty())
                            return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    template<typename F, typename... Args>
    auto add(F &&f, Args &&... args) noexcept {
#if __cplusplus>=201703L
        using r=std::invoke_result_t<F, Args...>;
#else
        using r = std::result_of_t<F(Args...)>;
#endif
        auto task_pack_ptr = std::make_shared<std::packaged_task<r()>>();
        if (m_copy) {
            task_pack_ptr = std::make_shared<std::packaged_task<r()>>([=]() -> r {
                return std::invoke(std::forward<F>(const_cast<F &&>(f)),
                                   std::forward<Args>(const_cast<Args &&>(args))...);
            });
        } else {
            task_pack_ptr = std::make_shared<std::packaged_task<r()>>([&]() -> r {
                return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            });
        }
        auto result = task_pack_ptr->get_future();
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            if (m_stop)
                return std::async(std::launch::async, [&]() {
                    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                });
            m_tasks.emplace([task_pack_ptr]() { (*task_pack_ptr)(); });
        }
        m_notify.notify_one();
        return result;
    }
    ~thread_pool() {
        {
            std::unique_lock<std::mutex> ulock(m_lock);
            m_stop = true;
        }
        m_notify.notify_all();
        for (auto &t : m_threads)
            t.join();
    }

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_lock;
    std::condition_variable m_notify;
    bool m_stop = false;
    bool m_copy = false;
public:
    thread_pool(const thread_pool&)= delete;
    thread_pool(thread_pool&&)= default;
    thread_pool&operator=(const thread_pool&)= delete;
    thread_pool&operator=(thread_pool&&)= default;
};

//using namespace std;
//struct node{
//    int v=0;
//    node(){
//        cout<<"constructor"<<endl;
//    }
//    ~node(){
//        cout<<"destructor"<<endl;
//    }
//    node(const node&r){
//        v=r.v;
//        cout<<"copy constructor"<<endl;
//    }
//    node(node&&r){
//        v=r.v;
//        cout<<"move constructor"<<endl;
//    }
//    node&operator=(const node&r){
//        v=r.v;
//        cout<<"copy assignment"<<endl;
//    }
//    node&operator=(node&&r){
//        v=r.v;
//        cout<<"move assignment"<<endl;
//    }
//    node&operator++(int){
//        v++;
//        return *this;
//    }
//};
//
//int main() {
//
//    thread_pool *pool = new thread_pool(4, true);
//    auto f = [](node &&a) {
//        this_thread::sleep_for(+1s);
//        cout << "v = " << a.v << endl;
//        return a.v;
//    };
//    auto test = node();
//    decltype(pool->add(f, std::move(test))) res[1001];
//
//    this_thread::sleep_for(+3s);
//
//    int count = 0;
//    while (test.v < 30) {
//        test++;
//        cout << "{" << test.v << "}" << endl;
//        res[count++] = pool->add(f, std::move(test));
//    }
//    delete pool;
//
//    for (auto i = 0; i < count; i++) {
//        cout << "get = " << res[i].get() << endl;
//    }
//
//    return 0;
//}

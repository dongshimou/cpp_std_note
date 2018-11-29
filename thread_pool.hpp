#include <iostream>

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <type_traits>

// template <typename T>
// void log(T&&t){
//     std::cout<<t<<std::endl;
// }
// template <typename T,typename ...Args>
// void log(T&&t,Args&&...args){
//     log(std::forward<T>(t));
//     log(std::forward<Args>(args)...);
// };

class thread_pool {
    std::queue<std::function<void(void)>>m_tasks;
    std::vector<std::thread>m_threads;
    std::mutex m_lock;
    std::condition_variable m_notify;
    bool m_stop = false;
    using ulock=std::unique_lock<std::mutex>;
public:
    explicit  thread_pool(size_t cap)noexcept{
        m_threads.emplace_back([&](){
            while (true){
                std::function<void(void)>task;
                {
                    //auto lock
                    ulock l(m_lock);
                    m_notify.wait(l, [this]() {
                        return m_stop || !m_tasks.empty();
                    });
                    if (m_stop && m_tasks.empty()) {
                        break;
                    }
                    task = m_tasks.front();
                    m_tasks.pop();
                    //auto unlock
                }
                task();
            }
        });
    }
    template <typename F,typename... Args>
    auto add(F&&f,Args&&...args)noexcept{
        using r=std::invoke_result_t<F,Args...>;
#if 1
        auto tmp=std::make_tuple(std::forward<Args>(args)...);
        auto todo=[&,t=std::move(tmp)]()->r{
                    return std::apply(std::forward<F>(f),t);
                };
#else
        auto todo=[&]()->r{ //can't forward args...
            return std::invoke(std::forward<F>(f),std::forward<Args>(args)...);
        };
#endif
        auto tpp=std::make_shared<std::packaged_task<r(void)>>(std::move(todo));
        {
            //auto lock
            ulock l(m_lock);
            if (m_stop) {
                return std::async(std::forward<F>(f), std::forward<Args>(args)...);
            }
            m_tasks.emplace([=]() {
                (*tpp)();
            });
            //auto unlock
        }
        m_notify.notify_one();
        return tpp->get_future();;
    }
    ~thread_pool(){
        {
            ulock l(m_lock);
            m_stop=true;
        }
        m_notify.notify_one();
        for(auto&t:m_threads){
            t.join();
        }
    }
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
//    thread_pool *pool = new thread_pool(4);
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

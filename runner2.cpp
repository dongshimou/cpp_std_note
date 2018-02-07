#include <iostream>

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <queue>
#include <future>
#include <type_traits>
#include <chrono>

struct runner {
    std::queue<std::function<void()>> func;

    runner() {}

    runner(const runner &) = delete;

    runner(runner &&) = default;

    runner &operator=(const runner &)= delete;

    runner &operator=(runner &&b)= default;

    ~runner()noexcept {
        run();
    }

    template<typename F, typename ...Args>
    auto binds(F &&f, Args &&...args)noexcept
    -> std::future<std::invoke_result_t<F,Args...>> {
        using r = std::invoke_result_t<F,Args...>;
        auto task = std::make_shared<std::packaged_task<r()>>(
                [&]() -> r {
                    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                }
        );
        auto result = task->get_future();
        func.push([task]() { (*task)(); });
        return result;
    };

    template<typename F, typename ...Args>
    auto add_callback(F &&f, Args &&...args)noexcept {
        return binds(std::forward<F>(f), std::forward<Args>(args)...);
    };

    void run() {
        while (!func.empty()) {
            auto i = std::move(func.front());
            func.pop();
            try {
                std::cout<<"pop 1===="<<std::endl;
                std::async(i);
            } catch (...) {
            }
        }
    }
};
template<typename T>
void echo(T &&t) {
    std::cout << t << std::endl;
}

using namespace std;
using namespace chrono;
struct node {
    int v = 0;

    int show() {
        std::cout << "sleep for v = " << v << std::endl;

        std::this_thread::sleep_for(+1s);
        std::this_thread::sleep_for(+1s);
        std::this_thread::sleep_for(+1s);

        return v;
    }
};

int main() {
    {
        node no;
        auto c = runner();
        auto res1 = c.add_callback([](int a, int b) {
            std::cout << "a + b =" << a + b << std::endl;
            return a + b;
        }, 1, 2);
        auto res2 = c.add_callback(&node::show, no);
        auto res3 = c.add_callback(echo<int>, 2333);

        std::cout<<"start run\n";
        auto t=std::thread([&c](){
            c.run();
        });
        std::cout<<"finish run\n";
        //run 之前调用 get() 会阻塞
        std::cout << "res1 "<<res1.get() << std::endl;
        std::cout << "res2 "<<res2.get() << std::endl;
        t.join();
    }
    std::cout << "delete c d" << std::endl;
    return 0;
}

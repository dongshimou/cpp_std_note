#include <iostream>

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <queue>
#include <future>

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
    void binds(F &&f, Args &&...args)noexcept {
//        暂时不写return
//        using r = typename std::result_of<F(Args...)>::type;
//        auto task = std::make_shared<std::packaged_task<r()>>(
//                [&]() -> r {
//                    return f(std::forward<Args>(args)...);
//                }
//        );
//        auto result = task->get_future();
//        func.push([task]() { (*task)(); });
        auto task = [&]() -> auto {
            return std::invoke(std::forward<F>(f),std::forward<Args>(args)...);
        };
        func.push(task);
    };

    template<typename F, typename ...Args>
    runner(F &&f, Args &&...args)noexcept {
        binds(std::forward<F>(f), std::forward<Args>(args)...);
    };

    template<typename F, typename ...Args>
    runner &add_callback(F &&f, Args &&...args)noexcept {
        binds(std::forward<F>(f), std::forward<Args>(args)...);
        return *this;
    };

    void run() {
        while (!func.empty()) {
            auto i = std::move(func.front());
            func.pop();
            try {
                i();
            } catch (...) {
            }
        }
    }
};

template<typename ...Args>
auto sum(Args &&...args) {
    return (args + ... +0);
}

void hello() {
    std::cout << "hello" << std::endl;
}

template<typename T>
void echo(T &&t) {
    std::cout << t << std::endl;
}

int main() {
    {
        auto c = runner([]() {
            std::cout << "fuck" << std::endl;
        });
        c.add_callback([](int a, int b) {
                    std::cout << "a + b =" << a + b << std::endl;
                }, 1, 2)
                .add_callback([]() {
                    std::cout << "you" << std::endl;
                })
                .add_callback([](int a, int b) {
                    return a + b;
                }, 2, 3)
                .add_callback(hello)
                .add_callback([]() {
                    sum(1, 2, 3, 4, 5, 6);
                })
                .add_callback(echo<std::string>, "world")
                .add_callback(sum<int, int, int>, 1, 2, 3);

        runner d = runner();
        d.add_callback([]() {
            std::cout << "ddd\n";
        });
        d.add_callback([](int a, int b) {
            if (b == 0) {
                throw (" div 0");
            }
        }, 1, 0).add_callback([]() {
            std::cout << "/0\n";
        });
//        d.run();

//        c.run();
    }
    std::cout << "delete c d" << std::endl;
    return 0;
}

#ifndef YANTHEMS_CALLBACK_HPP
#define YANTHEMS_CALLBACK_HPP

#include <iostream>

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <future>

#include "safe_queue.hpp"
class task_pool {
private:
    safe_queue<std::function<void()>> m_cb_func;
public:
    task_pool() = default;

    task_pool(const task_pool &) = delete;

    task_pool(task_pool &&) = default;

    task_pool &operator=(const task_pool &) = delete;

    task_pool &operator=(task_pool &&b) = default;

    template<typename F, typename ...Args>
    task_pool(F &&f, Args &&...args) noexcept {
        binds(std::forward<F>(f), std::forward<Args>(args)...);
    };

    ~task_pool() noexcept {
        run();
    }

private:
    template<typename F, typename ...Args>
    void binds(F &&f, Args &&...args) noexcept {
        auto tmp = std::make_tuple(std::forward<Args>(args)...);
        m_cb_func.push([&, t = std::move(tmp)]() -> auto {
            return std::apply(std::forward<F>(f), t);
        });
    };
public:
    template<typename F, typename ...Args>
    task_pool &add(F &&f, Args &&...args) noexcept {
        binds(std::forward<F>(f), std::forward<Args>(args)...);
        return *this;
    };

    void run() {
        while (!m_cb_func.empty()) {
            auto i = m_cb_func.front();
            m_cb_func.pop();
            try {
                i();
            } catch (...) {
            }
        }
    }
};

#endif //YANTHEMS_CALLBACK_HPP

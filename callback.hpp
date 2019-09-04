#ifndef YANTHEMS_CALLBACK_HPP
#define YANTHEMS_CALLBACK_HPP

#include <iostream>

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <queue>
#include <future>

class callback {
private:
    std::queue<std::function<void()>> m_cb_func;
public:
    callback() = default;

    callback(const callback &) = delete;

    callback(callback &&) = default;

    callback &operator=(const callback &) = delete;

    callback &operator=(callback &&b) = default;

    template<typename F, typename ...Args>
    callback(F &&f, Args &&...args) noexcept {
        binds(std::forward<F>(f), std::forward<Args>(args)...);
    };

    ~callback() noexcept {
        run();
    }

private:
    template<typename F, typename ...Args>
    void binds(F &&f, Args &&...args) noexcept {
        auto tmp = std::make_tuple(std::forward<Args>(args)...);
        m_cb_func.emplace([&, t = std::move(tmp)]() -> auto {
            return std::apply(std::forward<F>(f), t);
        });
    };
public:
    template<typename F, typename ...Args>
    callback &add(F &&f, Args &&...args) noexcept {
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

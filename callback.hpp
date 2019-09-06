
#ifndef CPPLIB_CALLBACK_HPP
#define CPPLIB_CALLBACK_HPP

#include <functional>
#include <tuple>
#include <type_traits>
class callback {
public:
    template<typename F, typename ...Args>
    constexpr auto add(F &&f, Args &&...args) {
        using r=std::invoke_result_t<F, Args...>;
        if constexpr (std::is_same_v < void, r >) {
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            return *this;
        } else {
            return std::make_tuple(*this, std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
        }
    }
};

#endif //CPPLIB_CALLBACK_HPP

#ifndef YANTHEMS_REGIST_HPP
#define YANTHEMS_REGIST_HPP

#include <iostream>
#include <any>
#include <map>
#include <functional>
#include <tuple>

template <typename T> struct type_helper : public type_helper<decltype(&T::operator())> {};
// type helper
template <typename ClassType, typename ReturnType, typename... Args>
struct type_helper<ReturnType(ClassType::*)(Args...) const> {
    // number of arguments
    enum : size_t { arity = sizeof...(Args) };
    // return type
    using result_type = ReturnType;
    // arg type
    template <size_t i> struct arg { using type = typename std::tuple_element<i, std::tuple<Args...>>::type; };
    using stdfunc_t = std::function<ReturnType(Args...)>;
};

class registers{
private:
    std::map<std::string,std::any>m_reg_funcs;
public:
    registers()= default;
    ~registers()= default;
    registers(const registers&)= delete;
    registers&operator=(const registers&)= delete;
    template <typename... Args>
    void run(const std::string&f,Args&&...args)noexcept {
        try {
            std::invoke(std::any_cast<std::function<void(Args...)>>(this->m_reg_funcs[f]), std::forward<Args>(args)...);
        } catch (const std::exception &e) {
            std::cout << __func__ << ":" << e.what() << std::endl;
        }
    }
    template <typename F>
    void bind(const std::string&name,F&&f)noexcept{
        //boost::function_types::parameter_types
        auto obj=typename type_helper<F>::stdfunc_t{std::forward<F>(f)};
        this->m_reg_funcs[name]=std::any(obj);
    }
};
#endif //YANTHEMS_REGIST_HPP

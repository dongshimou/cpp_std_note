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

class Test{
private:
    std::map<std::string,std::any>func;
public:
    Test() {

    }
    template <typename... Args>
    void Run(const std::string&f,Args&&...args)noexcept{
        auto ff=this->func[f];
//        if (ff.has_value()){
            try {
//                auto t=std::function<void(Args...)>();
                auto fff=std::any_cast<std::function<void(Args...)>>(ff);
#if 0
                fff(std::forward<Args>(args)...);
#else
                std::invoke(fff,std::forward<Args>(args)...);
#endif
            }catch (const std::exception&e) {
                std::cout<<__func__<<":"<<e.what()<<std::endl;
            }
//        }
    }

#if 1
    template <typename F>
    void regist(const std::string&name,F&&f)noexcept{
        //boost::function_types::parameter_types
        auto obj=typename type_helper<F>::stdfunc_t{std::forward<F>(f)};
        this->func[name]=std::any(obj);
    }
#else
    template <typename ...Args,typename F>
    void regist(const std::string&name,F&&f)noexcept{
        this->func[name]=std::any(std::function<void(Args...)>(std::forward<F>(f)));
    }
#endif
    void f1(int a){
        std::cout<<__func__<<":"<<a<<std::endl;
    }
    void f2(std::string b){
        std::cout<<__func__<<":"<<b<<std::endl;
    }
    void f3(const char*c){
        std::cout<<__func__<<":"<<c<<std::endl;
    }
    int f5(int a){
        return a+1;
    }

};
/*
int main() {
    Test t;

#if 0
    t.regist<int>("click", [](int a) {
        std::cout << "click " << a << std::endl;
    });

    t.regist<int, int>("add", [](int a, int b) {
        std::cout << "add " << a + b << std::endl;
    });
#else

    auto click=[](int a){
        std::cout<<"click "<<a<<std::endl;
    };

#if 0
    t.regist("click",[](int a){
        std::cout<<"click "<<a<<std::endl;
    });
#else
    t.regist("click",std::move(click));

#endif

#endif
    t.Run("click", 3);
    t.Run("click",5);


    return 0;
}
*/

#include <iostream>
#include <any>
#include <map>
#include <functional>
#include <tuple>

class Test{
private:
    std::map<std::string,std::any>func;
public:
    Test() {
        this->regist<int>("f1",[=](int a){
            this->f1(a);
        });
        this->regist<std::string>("f2",[=](std::string a){
            this->f2(std::forward<std::string>(a));
        });
        this->regist<const char*>("f3",[=](const char* a){
            this->f3(a);
        });
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


    template <typename ...Args,typename F>
    void regist(const std::string&name,F&&f)noexcept{
        this->func[name]=std::any(std::function<void(Args...)>(std::forward<F>(f)));
    }

#if 0
    template <typename F>
    void regist(const std::string&name,F&&f)noexcept{
        //boost::function_types::parameter_types
        //get F Args
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
    t.Run("f1", 1);
    std::string b{"aaa"};
    t.Run("f2", static_cast<std::string>(b));
    t.Run("f2", b);
    t.Run("f2", std::string("bbb"));
    t.Run("f2", "abc");
    const char *cc = "ccc";
    t.Run("f3", cc);
    t.Run("f3", static_cast<const char *>(cc));
    t.Run("f4");

    t.regist<int>("click", [](int a) {
        std::cout << "click " << a << std::endl;
    });

    t.regist<int, int>("add", [](int a, int b) {
        std::cout << "add " << a + b << std::endl;
    });

    t.Run("click", 3);
    t.Run("add", 1, 4);

    return 0;
}
*/

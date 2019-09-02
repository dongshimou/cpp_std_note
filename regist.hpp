#include <iostream>
#include <any>
#include <map>
#include <functional>

class Test{
private:
    std::map<std::string,std::any>func;
public:
    Test() {
        this->func["f1"] = std::any(
                std::function<void(int)>([=](int a) {
                    this->f1(std::forward<int>(a));
                })
        );
        this->func["f2"] = std::any(
                std::function<void(std::string)>([=](std::string b) {
                    this->f2(std::forward<std::string>(b));
                })
        );
        this->func["f3"] = std::any(
                std::function<void(const char *)>([=](const char *c) {
                    this->f3(std::forward<const char *>(c));
                })
        );
    }
    template <typename... Args>
    void Run(const std::string&f,Args&&...args)noexcept{
        auto ff=this->func[f];
        if (ff.has_value()){
            try {
                auto t=std::function<void(Args...)>();
                auto fff=std::any_cast<std::function<void(Args...)>>(ff);
                fff(std::forward<Args>(args)...);
            }catch (const std::exception&e) {
                std::cout<<__func__<<":"<<e.what()<<std::endl;
            }
        }
    }
    void f1(int a){
        std::cout<<__func__<<":"<<a<<std::endl;
    }
    void f2(std::string b){
        std::cout<<__func__<<":"<<b<<std::endl;
    }
    void f3(const char*c){
        std::cout<<__func__<<":"<<c<<std::endl;
    }

};
/*
int main() {
    Test t;
    t.Run("f1",1);
    std::string b{"aaa"};
    t.Run("f2", static_cast<std::string>(b));
    t.Run("f2",b);
    t.Run("f2",std::string("bbb"));
    t.Run("f2","abc");
    const char* cc="ccc";
    t.Run("f3",cc);
    t.Run("f3", static_cast<const char*>(cc));
    return 0;
    
//f1:1
//f2:aaa
//Run:bad any_cast
//f2:bbb
//Run:bad any_cast
//Run:bad any_cast
//f3:ccc
}
*/

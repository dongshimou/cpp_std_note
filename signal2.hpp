#include <functional>
#include <vector>
#include <future>
namespace pp{
    enum struct execute{
        queue=0,
        async=1,
        defer=2,
    };
    template<typename T>
    class signal final {
        using FT=std::function<T>;

    private:
        std::vector<FT> slots;
        execute policy;
        std::vector<std::future<void>>res;
    public:
        signal(const signal &) = delete;

        signal(signal &&other) = default;

        signal &operator=(const signal &) = delete;

        signal &operator=(signal &&other) = default;

        signal &operator=(FT &&t) {
            return this->connect(std::forward<FT>(t));
        }

        signal &connect(FT &&t) {
            slots.emplace_back(std::forward<FT>(t));
            return *this;
        }

        explicit signal(execute exe=execute::queue):policy(exe) {}

        ~signal() {}

        template<typename... Args>
        constexpr auto operator()(Args &&... args) noexcept ->
        std::vector<std::future<std::result_of_t<FT(Args &&...)>>> {
            using rt=std::result_of_t<FT(Args &&...)>;
            std::vector<std::future<rt>>res;
            switch (policy){
                case execute::queue:{
                    for (const auto &s : slots) {
                        std::packaged_task<rt()>temp([&](){
                            return std::invoke(s,std::forward<Args>(args)...);
                        });
                        res.emplace_back(temp.get_future());
                        temp();
                    }
                    break;
                }
                case execute::async:{
                    for (const auto &s : slots) {
                        res.emplace_back(std::async(std::launch::async,s,std::forward<Args>(args)...));
                    }
                    break;
                }
                case execute::defer:{
                    for (const auto &s : slots) {
                        res.emplace_back(std::async(std::launch::deferred,s,std::forward<Args>(args)...));
                    }
                    break;
                }
            }
            return res;
        }
    };
}


/*
#include <tuple>
#include <vector>
#include <functional>
#include <iostream>
#include "signal.h"
#include <chrono>

struct Logger{
    void log(int a){
        std::cout<<"log "<<a<<std::endl;
    }
};
struct Label{
    pp::signal<void(int)>click=pp::signal<void(int)>(pp::execute::async);//defer
    Label(){}
};

int main() {

    Label l1;
    Logger l2;
    Logger l3;
    Logger l4;

    l1.click=[&l2](int a){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<"l2 ";
        l2.log(a);
    };
    l1.click=[&l3](int a){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout<<"l3 ";
        l3.log(a);
    };
    l1.click=[&l4](int a){
        std::cout<<"l4 ";
        l4.log(a);
    };

    l1.click(1);
//    auto res=l1.click(1);
    std::cout<<"==[~res]==\n";

    return 0;
}
*/

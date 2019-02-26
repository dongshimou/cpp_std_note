
#ifndef YANTHEMS_LOGGER_HPP
#define YANTHEMS_LOGGER_HPP
#include <sstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <mutex>
namespace logger {

class time;
class time_init;

class time_init {
private:
    time_init() {
#ifdef __linux
        //linux设置时区 环境变量
        auto tz = getenv("TZ");
//        printf("TZ : %s\n", tz);
        //tzset();
        if (tz == NULL) {
            putenv(const_cast<char*>("TZ=Asia/Shanghai"));
//            printf("TZ : %s\n", getenv("TZ"));
        }
#endif
    }

    static inline time_init *ptr = nullptr;
public:
    static time_t get_now_t() {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    static time_init *get() {
        if (ptr == nullptr) {
            ptr = new time_init;
        }
        return ptr;
    }

    std::string get_now_format(const std::string &format) {
        auto t = get_now_t();
        // ref: https://zh.cppreference.com/w/cpp/io/manip/put_time
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), format.c_str());
        return ss.str();
    }
};
class time {
public:
    static std::string get_now_str() {
        return time_init::get()->get_now_format("%Y-%m-%d %H:%M:%S");
    }

    static std::string get_date_str() {
        return time_init::get()->get_now_format("%Y-%m-%d");
    }

    static std::string get_time_str() {
        return time_init::get()->get_now_format("%H:%M:%S");
    }

    static std::string get_now_format(const std::string &format) {
        return time_init::get()->get_now_format(format.c_str());
    }

    static std::time_t get_time_t() {
        return time_init::get()->get_now_t();
    }
};
#define LINE2STR(x) #x
#define LINE2STRHELP(x) LINE2STR(x)
#define POS ("[" __FILE__ ":line " LINE2STRHELP(__LINE__) "]")
#define DATE (time::get_date_str())
#define TIME (time::get_time_str())

template <typename T>
inline void logf(T&&t){
    std::cout<<t<<std::endl;
}
template <typename T,typename ...Args>
inline void logf(T&&t,Args&&...args){
    std::cout<<std::forward<T>(t)<<' ';
    logf(std::forward<Args>(args)...);
};

template <typename ...Args>
inline void log(Args&&...args){
    std::unique_lock<mutex>ul;
    logf(std::forward<Args>(args)...);
}
template <typename... Args>
inline void debug(Args&&...args){
    auto prefix=std::string("[DEBUG]");
    prefix+=" [";
    prefix+=DATE;
    prefix+=" ";
    prefix+=TIME;
    prefix+="]";
    logger::log(prefix,std::forward<Args>(args)...);
}


}
#endif //YANTHEMS_LOGGER_HPP

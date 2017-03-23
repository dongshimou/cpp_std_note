## future_error

(c++11起)

(class)
future对象
[cplusplus](http://www.cplusplus.com/reference/future/future_error/)
[cppreference](http://zh.cppreference.com/w/cpp/thread/future_error)


catch `future`相关的错误

```
#include <future>
#include <iostream>
 
int main()
{
    std::future<int> empty;
    try {
        int n = empty.get();
    } catch (const std::future_error& e) {
        std::cout << e.code() << '\n' << e.what() << '\n';
    }
}
```
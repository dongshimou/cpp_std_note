## async

(c++11起)

(function)
创建一个异步操作,返回一个`future`对象.
[cplusplus](http://www.cplusplus.com/reference/future/async/)
[cppreference](http://zh.cppreference.com/w/cpp/thread/async)

例如:
`launch::async`

```
#include<future>
#include<cassert>
#include<iostream>
#include<chrono>
using namespace std;
int sum = 0;
auto func(int a, int b) {
    sum = a + b;
    return sum;
}
int main() {
    //创建一个异步操作a(future对象),其中第一参数`launch::async`(也是默认参数可以不写)标志着a为异步立即执行,实现上的实现有可能是创建一个新的线程去执行,第二个参数`func`为需要异步执行的函数,第三个和第四个参数为函数`func`所需的参数,如果你需要异步执行的函数没有参数则不要添加后续的参数,如`auto a=async(func,1,2);`
    auto a = async(launch::async, func, 1, 2);
    //当前线程休眠1s
    this_thread::sleep_for(+1s);
    cout << sum << '\n';
    cout << a.get() << '\n';
    getchar();
    return 0;
}
```
`launch::deferred`

```
#include<future>
#include<cassert>
#include<iostream>
#include<chrono>
using namespace std;
int sum = 0;
auto func(int a, int b) {
    this_thread::sleep_for(+1s);
    sum = a + b;
    return sum;
}
int main() {
    //创建一个异步操作,`launch::deferred`表明这是一个当前线程的延迟求值操作,这是一个惰性的方法,只会在你第一次想要得到结果的时候执行这个函数.
    auto a = async(launch::deferred, func, 1, 2);
    cout << sum << '\n';
    cout << a.get() << '\n';
    cout << sum << '\n';
    getchar();
    return 0;
}
```
`launch::async|launch::deferred`
在某种程度上自动选择,这取决于系统库的实现
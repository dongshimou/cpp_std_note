## future

(c++11起)

(class)
future对象
[cplusplus](http://www.cplusplus.com/reference/future/future/)
[cppreference](http://zh.cppreference.com/w/cpp/thread/future)

  * 一个异步操作(通过`std::async`,`std::packaged_task`或`std::promise`创建)可以提供一个`std::future`对象给这个异步操作的创建者. 
  * 异步操作的创建者可以使用多个方法来从`std::future`中查询,等待或提取一个值.如果异步操作尚未提供一个值,这些方法可能会阻塞.
  * 当异步操作已准备好发送结果给创建者时,它可以通过修改与创建者的`std::future`相关的"共享状态"(例如`std::promise::set_value`)来完成发送.

#### share
```
//msvc141 的实现
shared_future<_Ty> share() {
    return (shared_future<_Ty>(_STD move(*this)));
}
```
返回一个`*this`相关的`shared_future`对象

#### get

获得结果

#### valid

返回这个对象是否被共享

#### wait

等待这个对象直到结果可用

#### wait_for

等待指定时间,然后返回一个状态`future_status`

#### wait_until

等待到某个时间点,然后返回一个状态

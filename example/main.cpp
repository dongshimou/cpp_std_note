
#include "../callback.hpp"
#include "../logger.hpp"
#include "../spin_lock.h"
#include "../thread_pool.hpp"
#include <iostream>
#include <chrono>

#include <thread>
#include <chrono>
int main(){

    thread_pool pool{4};

    pool.add([](){
        std::cout<<"fuck"<<std::endl;
    });

    class Test:public callback{

    }t;
    t.add([](){
        logger::debug(POS,FUN,1);
    }).add([](){
        logger::debug(POS,FUN,2);
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
}
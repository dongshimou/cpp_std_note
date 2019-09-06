
#include "../callback.hpp"
#include "../logger.hpp"
#include <iostream>
#include <chrono>

#include <thread>
#include <chrono>
int main(){


    class Test:public callback{

    }t;
    t.add([](){
        logger::debug(POS,FUN,1);
    }).add([](){
        logger::debug(POS,FUN,2);
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
}
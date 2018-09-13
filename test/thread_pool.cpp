#include "../src/thread_pool.h"
#include <chrono>
#include <cstdio>

using namespace std::chrono_literals;

int main(){
    Nuke::ThreadExecutor exe((2));
    int count = 5;
    std::atomic<int> finished;
    finished.store(0);
    for(int i = 0; i < count; i++){
        exe.add_task([&finished, i](){
            printf("%c start\n", 'a' + i);
            std::this_thread::sleep_for(1s);
            printf("%c end\n", 'a' + i);
            finished ++;
        });
    }
    exe.add_task([&](){
        while(finished.load() != count){
            std::this_thread::yield();
        }
        puts("END");
        exe.stop();
    });
    exe.wait();
}

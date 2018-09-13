#include <thread>
#include <mutex>
#include <condition_variable>


namespace Nuke{
template<typename T>
struct BlockingQueue{
    bool check_full_unguard(){
        return ((tail + 1) % capacity) == head % capacity;
    }
    bool check_empty_unguard(){
        return (tail % capacity) == (head % capacity);
    }
    void push(T && x){
        std::unique_lock<std::mutex> lk(mtx);
        while (check_full_unguard()){
            cv_full.wait(mut);
        }
        dat[tail] = x;
        tail = (tail + 1) % capacity;
        cv_empty.notify_one();
    }
    T pop(){
        std::unique_lock<std::mutex> lk(mtx);
        while (check_empty_unguard()){
            cv_empty.wait(mut);
        }
        T x = std::move(dat[head]);
        head = (head + 1) % capacity;
        cv_full.notify_one();
        return x;
    }
    void clear(){
        mut.lock();
        head = end = 0;
        cv_empty.notify_all();
    }
    BlockingQueue(size_t _capacity) : capacity(_capacity){
        dat = new T[capacity];
    }
    ~BlockingQueue(){
        delete [] dat;
    }
    T * dat;
    size_t head, tail, capacity;
    std::mutex mut;
    std::condition_variable cv_empty;
    std::condition_variable cv_full;
};
}
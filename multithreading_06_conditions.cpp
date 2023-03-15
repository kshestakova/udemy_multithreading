#include <condition_variable>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    bool ready = false;
    std::cout << ready << std::endl;

    std::condition_variable condition;
    std::mutex mtx;

    // anonymous function which changes the value of variable ready after 2 seconds
    // at the start it is waiting while the main thread releases mutex mtx
    std::thread t1([&](){
        // creates a lock and waits while mtx will be released by the main thread
        std::unique_lock<std::mutex> lockT1(mtx);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        ready = true;
        lockT1.unlock();
        // notifies one waiting thread when mtx is released
        condition.notify_one();
    });

    // created a lock with the same mutex, locked it
    std::unique_lock<std::mutex> lockT2(mtx);

    // main thread will be waiting while lock mutex will be released 
    while(!ready)
    {
        // released the mutex mtx
        condition.wait(lockT2);
    }

    // more attractive way - second parameter as predicate, lambda expression
    // condition will be waiting until result becomes true
    // condition.wait(lockT2, [&] () {return ready;});

    // after releasing a mutex mtx by the t1 thread 
    // the main thread continues running

    std::cout << ready;

    t1.join();
}
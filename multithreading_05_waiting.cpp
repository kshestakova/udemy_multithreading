#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    // thread-safe variable which is used in different threads
    std::atomic<bool> ready = false;

    std::cout << ready << std::endl;

    // anonymous function which changes the value of variable ready after 2 seconds
    std::thread t1([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        ready = true;
    });

    t1.join();

    // main thread will be waiting while t1 changes the value of ready 
    while(!ready)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << ready;
}
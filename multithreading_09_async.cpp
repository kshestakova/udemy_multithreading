#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int foo(int id)
{
    for(int i = 0; i < 10; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << id << " " << i << std::endl;
    }
    return id;
}

int main()
{
    // even if we use launch::async, threads will be running one after another by design
    std::async(std::launch::async, foo, 1);
    std::async(std::launch::async, foo, 2);

    // and if we set the result to future object, they will be runnind at parallel
    std::future<int> f1 = std::async(std::launch::async, foo, 3);
    std::future<int> f2 = std::async(std::launch::async, foo, 4);


    std::cout << f1.get() << std::endl;
    std::cout << f2.get() << std::endl;

    // with launch::deferred they are also running one after another
    // but here we receive f3.get() result just after tread finished working 9not after both threads)

    std::future<int> f3 = std::async(std::launch::deferred, foo, 5);
    std::future<int> f4 = std::async(std::launch::deferred, foo, 6);
    
    std::cout << f3.get() << std::endl;
    std::cout << f4.get() << std::endl;

    // also in this laptop I can run 16 threads: 8 cores * 2 because of hyperthrerading technology
    std::cout << std::thread::hardware_concurrency() << std::endl;
}
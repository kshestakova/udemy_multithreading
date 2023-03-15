#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

void work()
{
    for(int i = 0; i < 20; ++i) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Work: " << i << std::endl;
    }
}

int main()
{
    std::thread t1(work);
    std::thread t2(work);

    // run at parallel: when one sleeps, another works
    t1.join();
    t2.join();

    int count = 0;
    const int ITERATIONS = 1E6;

    std::thread l1([&count](){
        for(int i = 0; i < ITERATIONS; ++i) 
    {
        ++count;
    }
    });

    std::thread l2([&count](){
        for(int i = 0; i < ITERATIONS; ++i) 
    {
        ++count;
    }
    });

    l1.join();
    l2.join();

    // shows that increment doesn't work every time in both threads - count < 1e6 * 2
    std::cout << count << std::endl;

    // quick fix - using thread-safe atomic. atomic<int> equals to atomic_int from c++ 11
    std::atomic<int> acount = 0;
    auto foo = [&acount](){
        for(int i = 0; i < ITERATIONS; ++i) 
    {
        ++acount;
    }
    };

    std::thread l3(foo);
    std::thread l4(foo);

    l3.join();
    l4.join();

    // acount == 1e6 * 2 
    std::cout << acount << std::endl;


    std::mutex mtx;
    count = 0;

    // another solution - threadSafe function with mutex, which must be locked before data would be changed and unlocked after that
    // woth mutex only one thread can access count at a time
    auto threadSafeFoo = [&count, &mtx](){
        for(int i = 0; i < ITERATIONS; ++i) 
        {
            mtx.lock();
            ++count;
            mtx.unlock();
        }
    };

    // disadvantage of mutexes - if we receive exception in critical section between locks, unlock wouldn't work

    std::thread ts1(threadSafeFoo);
    std::thread ts2(threadSafeFoo);
    ts1.join();
    ts2.join();

    // now count also equals 1e6 * 2
    std::cout << count << std::endl;

    auto lockGuargFoo = [&count, &mtx](){
        for(int i = 0; i < ITERATIONS; ++i) 
        {
            // lock_guard lock section till the bracke '}' and automatically released when the program exits this section
            const std::lock_guard<std::mutex> guard(mtx);
            ++count;
        }
    };

    // we could also use unique_lock which has possibility to lock, unlock, try_lock etc. 

    std::thread lg1(lockGuargFoo);
    std::thread lg2(lockGuargFoo);
    lg1.join();
    lg2.join();

    std::cout << count << std::endl;
}
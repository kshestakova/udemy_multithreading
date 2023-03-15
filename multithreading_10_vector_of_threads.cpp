#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

std::mutex g_mtx;

int foo(int id)
{
    // global mutex just for pretty strings
    std::unique_lock<std::mutex> lock(g_mtx);
    std::cout << "Foo# " << id  << std::endl;
    lock.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    lock.lock();
    std::cout << id << " finished running" << std::endl;
    lock.unlock();

    return id;
}

int main()
{
    //  in this laptop we can run 16 threads: 8 cores * 2 because of hyperthrerading technology
    int cores = std::thread::hardware_concurrency();

    // since future doesn't have copy constructor, we will use shared_future here
    std::vector<std::shared_future<int>> futures;

    // in this case threads are running asynchronously
    for(int i = 0; i < cores; ++i) 
    {
        std::shared_future<int> f = std::async(std::launch::async, foo, i);
        futures.push_back(f);
    }

    for(auto f : futures)
    {
        std::cout << f.get() << std::endl;
    }
}
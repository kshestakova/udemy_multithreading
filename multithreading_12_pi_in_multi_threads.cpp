#include <chrono>
#include <cmath>
#include <future>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

// we can compare how long it takes to calculate the value of Pi in 1 or 16 threads
// const int THREADS = 1;
const int THREADS = std::thread::hardware_concurrency();
const int TERMS = 1E8;

double leibnitz(int terms, int start, int skip) 
{
    double sum = 0;
    for(int i = start; i < terms; i += skip)
    {
        sum += 1. / (i * 2 + 1)* pow(-1, i);
    }
    return sum;
}

int main()
{
    std::vector<std::shared_future<double>> futures;

    auto start = std::chrono::steady_clock::now();

    for(int i = 0; i < THREADS; ++i) 
    {
        std::shared_future<double> f = std::async(std::launch::async, leibnitz, TERMS, i, THREADS);
        futures.push_back(f);
    }

    double sum = .0;
    for(auto f : futures)
    {
        sum += f.get();
    }
    sum *= 4;

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << std::setprecision(15) << M_PI << std::endl;
    std::cout << std::setprecision(15) << sum << std::endl;
    std::cout << "It takes " << duration << " milliseconds. Threads: " << THREADS << std::endl;
}
#include <future>
#include <iomanip>
#include <iostream>
#include <thread>

double leibnitz(int terms) 
{
    int denominator = 1;
    double sum = 0;
    int sign = 1;
    for(int i = 0; i < terms; ++ i)
    {
        sum += 1. / denominator * sign;
        sign *= -1;
        denominator += 2;
    }
    return sum * 4;
}

int main()
{
    std::promise<double> promise;

    // lambda function which receives the calculation result
    auto promiseLambda = [&](int terms)
    {
        auto result = leibnitz(terms);
        promise.set_value(result);
    };

    std::thread t1(promiseLambda, 1E9);

    // connecting future with promise
    std::future<double> future = promise.get_future();

    // future.get() will be waiting for the result of calculation - promise.set_value(result);
    std::cout << std::setprecision(15) << future.get() << std::endl;

    // it doesn't matter where to use join() - before or after get()
    // because join() in fact returns when t1 execution is completed
    // and also blocks the main thread until t1 finishes its execution
    t1.join();
}
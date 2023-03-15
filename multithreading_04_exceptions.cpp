#include <future>
#include <iomanip>
#include <iostream>
#include <thread>

double leibnitz(int terms) 
{
    if(terms < 1)
    {
        throw std::runtime_error("Terms mustn't be less than 1");
    }

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

    auto promiseLambda = [&](int terms)
    {
        // call set_value or set_exception - 
        // in other case future.get() will be waiting permanently
        try
        {
            auto result = leibnitz(terms);
            promise.set_value(result);
        }
        catch(...)
        {
            promise.set_exception(std::current_exception());
        }
    };

    //std::thread t1(promiseLambda, 1E9);
    std::thread t1(promiseLambda, 0);
    std::future<double> future = promise.get_future();

    try
    {
        std::cout << std::setprecision(15) << future.get() << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what(); 
    }

    t1.join();
}
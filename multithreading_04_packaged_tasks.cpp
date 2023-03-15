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
    std::packaged_task<double(int)> task1(leibnitz);
    std::future<double> future = task1.get_future();

    //std::thread t1(task1, 1E9);

    // also we can use move(task1) - it doesn't have copy semantics and also wouldn't be used below
    std::thread t1(ref(task1), 0);

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
#include <iostream>
#include <mutex>
#include <thread>

const int ITERATIONS = 1E6;

//callable class
class App
{
    private:
        int count;
        std::mutex mtx;

    public:
        int getCount() {return count;}
        void operator()() {
            for(int i = 0; i < ITERATIONS; ++i)
            {
                const std::lock_guard<std::mutex> lg(mtx);
                ++count;
            }
        }
        void resetCount() {count = 0;}
};

int main()
{
    App one;
    one();
    std::cout << one.getCount() << std::endl;
    one.resetCount();

    // since mutex doesn't use copy and move semantics, it doesn't have copy constructor
    // that's why we use reference to object instead of object copy as a parameter of the function
    std::thread t1(std::ref(one));
    std::thread t2(std::ref(one));

    t1.join();
    t2.join();

    std::cout << one.getCount() << std::endl;

}
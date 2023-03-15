#include <condition_variable>
#include <chrono>
#include <iostream>
#include <queue>
#include <thread>

const int NUMBER_OF_ELEMENTS = 1E3;

template<typename E>
class BlockingQueue : std::queue<E>
{
    private:
        std::queue<E> m_q;
        int m_max_size;

    public:
        BlockingQueue<E>(int max_size) : m_max_size(max_size)
        {}

        void push(E el) 
        {
            m_q.push(el);
            std::cout << "push ";
        }

        E pop()
        {
            E el = m_q.front();
            m_q.pop();
            std::cout << "pop ";
            return el;
        }
};

int main()
{
    BlockingQueue<int> bq(NUMBER_OF_ELEMENTS);

    std::thread t1(&BlockingQueue<int>::push, bq, 7);
    std::thread t2(&BlockingQueue<int>::pop, bq);

    t1.join();
    t2.join();


    // it is possible to receive segmentation fault error here, 
    // especially for big numbers of on big NUMBER_OF_ELEMENTS 

    std::thread t3([&](){
        for(int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
        {
            bq.push(i);
        }
    });

    std::thread t4([&](){
        for(int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
        {
            bq.pop();
        }
    });

    t3.join();
    t4.join();
}
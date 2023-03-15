#include <condition_variable>
#include <chrono>
#include <iostream>
#include <queue>
#include <thread>

const int NUMBER_OF_ELEMENTS = 1E3;

template<typename E>
class BlockingQueue 
{
    private:
        std::queue<E> m_q;
        int m_max_size;
        std::mutex m_mtx;
        std::condition_variable m_cv;

    public:
        BlockingQueue<E>(int max_size) : m_max_size(max_size)
        {}

        void push(E el) 
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_cv.wait(lock, [this](){ return m_q.size() < m_max_size; });
            
            m_q.push(el);
            std::cout << " push ";

            lock.unlock();
            m_cv.notify_all();
        }

        E front()
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_cv.wait(lock, [this](){ return !m_q.empty(); });
            std::cout << " front ";

            return m_q.front();
        }

        void pop()
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_cv.wait(lock, [this](){ return !m_q.empty(); });

            m_q.pop();
            std::cout << " pop ";

            lock.unlock();
            m_cv.notify_one();

        }

        int getSize() 
        { 
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_q.size(); 
        } 
};

int main()
{
    BlockingQueue<int> bq(50);

    std::thread t1([&](){
        for(int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
        {
            bq.push(i);
            std::cout << bq.getSize();
        }
    });

    std::thread t2([&](){
        for(int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
        {
            auto item = bq.front();
            bq.pop();
            std::cout << bq.getSize();
        }
    });

    t1.join();
    t2.join();
}
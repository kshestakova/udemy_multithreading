#include <chrono>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

const int NUMBER_OF_THREADS = 100;
const int BLOCKING_QUEUE_SIZE = 10;

std::mutex g_mtx;

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

int foo(int id)
{
    // global mutex just for pretty strings
    std::unique_lock<std::mutex> lock(g_mtx);
    std::cout << "\nFoo# " << id  << std::endl;
    lock.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    lock.lock();
    std::cout << id << " finished running  " << std::endl;
    lock.unlock();

    return id;
}

int main()
{
    BlockingQueue<std::shared_future<int>> futures(BLOCKING_QUEUE_SIZE);

    // something like main thread which manages other threads creation
    // new thread can be created only when there is empty slot in the BlockingQueue
    std::thread t([&](){
        for(int i = 0; i < NUMBER_OF_THREADS; ++i) 
        {
            std::shared_future<int> f = std::async(std::launch::async, foo, i);
            futures.push(f);
        }
    });


    for(int i = 0; i < NUMBER_OF_THREADS; ++i) 
    {
        std::shared_future<int> f = futures.front();
        int value = f.get();
        futures.pop();

        std::cout << value << std::endl;
    }

    t.join();
}
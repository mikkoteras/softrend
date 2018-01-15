#include "thread_pool.h"

#if THREAD_POOL_REUSE_THREADS

    template<typename parent_class>
    thread_pool<parent_class>::thread_pool(parent_class *parent, unsigned num_threads) :
        parent(parent),
        thread_busy(num_threads, false) {

        for (unsigned i = 0; i < num_threads; ++i)
            threads.push_back(std::thread(&thread_pool::loop, this, i));
    }

    template<typename parent_class> thread_pool<parent_class>::~thread_pool() {
        std::unique_lock<std::mutex> lock(work_mutex);
        stopping = true;
        threads_activated.notify_all();
        lock.unlock();

        while (!threads.empty()) {
            threads.back().join();
            threads.pop_back();
        }
    }

    template<typename parent_class>
    void thread_pool<parent_class>::execute(void(parent_class::*func)(unsigned thread_index)) {
        std::unique_lock<std::mutex> lock(work_mutex);
        work_function = func;
        num_threads_busy = static_cast<unsigned>(threads.size());

        for (unsigned i = 0; i < thread_busy.size(); ++i)
            thread_busy[i] = true;

        threads_activated.notify_all();
        threads_completed.wait(lock);
        work_function = nullptr;
    }

    template<typename parent_class> void thread_pool<parent_class>::loop(unsigned thread_index) {
        std::unique_lock<std::mutex> lock(work_mutex);

        while (!stopping) {
            if (!thread_busy[thread_index])
                threads_activated.wait(lock);

            if (thread_busy[thread_index] && work_function) {
                lock.unlock();
                (parent->*work_function)(thread_index);
                lock.lock();
                thread_busy[thread_index] = false;

                if (!(--num_threads_busy))
                    threads_completed.notify_one();
            }
        }
    }

#else // !THREAD_POOL_REUSE_THREADS

    template<typename parent_class>
    thread_pool<parent_class>::thread_pool(parent_class *parent, unsigned num_threads) :
        parent(parent),
        threads(num_threads),
        num_threads(num_threads) {
    }

    template<typename parent_class> thread_pool<parent_class>::~thread_pool() {
    }

    template<typename parent_class>
    void thread_pool<parent_class>::execute(void(parent_class::*func)(unsigned thread_index)) {
        for (unsigned i = 0; i < num_threads; ++i)
            threads.emplace(threads.cbegin() + i, std::move(std::thread(func, parent, i)));

        for (unsigned i = 0; i < num_threads; ++i)
            threads[i].join();
    }

#endif

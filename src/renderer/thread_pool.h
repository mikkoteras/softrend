#ifndef THREAD_POOL
#define THREAD_POOL

#include "config/optimizations.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

template<typename parent_class> class thread_pool {
public:
    thread_pool() = delete;
    thread_pool(parent_class *parent, unsigned num_threads);
    ~thread_pool();

    void execute(void(parent_class::*func)(unsigned thread_index));

private:
    parent_class *parent;
    std::vector<std::thread> threads;

#if THREAD_POOL_REUSE_THREADS
    void loop(unsigned thread_index);
    
    std::mutex work_mutex;
    std::condition_variable threads_activated;
    std::condition_variable threads_completed;
    std::vector<bool> thread_busy;
    unsigned num_threads_busy = 0;
    void(parent_class::*work_function)(unsigned thread_index) = nullptr;
    bool stopping = false;
#else
    unsigned num_threads;
#endif
};

#include "thread_pool.cpp.h"

#endif

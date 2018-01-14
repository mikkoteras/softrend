#include "thread_pool.h"

template<typename parent_class> thread_pool<parent_class>::thread_pool(parent_class *parent, size_t num_workers) :
    parent(parent),
    thread_busy(num_workers, false) {

    for (size_t i = 0; i < num_workers; ++i)
        threads.push_back(thread(&thread_pool::loop, this, i));
}

template<typename parent_class> thread_pool<parent_class>::~thread_pool() {
    unique_lock<mutex> lock(work_mutex);
    stopping = true;
    threads_activated.notify_all();
    lock.unlock();

    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
}

template<typename parent_class> void thread_pool<parent_class>::execute(void(parent_class::*func)(size_t thread_index)) {
    unique_lock<mutex> lock(work_mutex);
    work_function = func;
    num_threads_busy = threads.size();

    for (unsigned i = 0; i < thread_busy.size(); ++i)
        thread_busy[i] = true;

    threads_activated.notify_all();
    threads_completed.wait(lock);
    work_function = nullptr;
}

template<typename parent_class> void thread_pool<parent_class>::loop(size_t thread_index) {
    unique_lock<mutex> lock(work_mutex);

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

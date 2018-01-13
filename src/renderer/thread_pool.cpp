#include "thread_pool.h"
#include "scene.h"

using namespace std;

thread_pool::thread_pool(scene *parent_scene, size_t num_workers) :
    parent_scene(parent_scene),
    thread_busy(num_workers, false) {

    for (size_t i = 0; i < num_workers; ++i)
        threads.push_back(thread(&thread_pool::loop, this, i));
}

thread_pool::~thread_pool() {
    unique_lock<mutex> lock(work_mutex);
    stopping = true;
    threads_activated.notify_all();
    lock.unlock();

    while (!threads.empty()) {
        threads.back().join();
        threads.pop_back();
    }
}

void thread_pool::execute(void(scene::*func)(size_t thread_index)) {
    unique_lock<mutex> lock(work_mutex);
    work_function = func;
    num_threads_busy = threads.size();

    for (unsigned i = 0; i < thread_busy.size(); ++i)
        thread_busy[i] = true;

    threads_activated.notify_all();
    threads_completed.wait(lock);
    work_function = nullptr;
}

void thread_pool::loop(size_t thread_index) {
    unique_lock<mutex> lock(work_mutex);

    while (!stopping) {
        if (!thread_busy[thread_index])
            threads_activated.wait(lock);

        if (thread_busy[thread_index] && work_function) {
            lock.unlock();
            (parent_scene->*work_function)(thread_index);
            lock.lock();
            thread_busy[thread_index] = false;

            if (!(--num_threads_busy))
                threads_completed.notify_one();
        }
    }
}

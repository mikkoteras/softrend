#include "thread_pool.h"
#include "scene.h"

using namespace std;

thread_pool::thread_pool(scene *parent_scene, int num_workers) :
    parent_scene(parent_scene),
    worker_running(num_workers, false) {

    for (size_t i = 0; i < num_workers; ++i)
        workers.push_back(thread(&thread_pool::loop, this, i));
}

thread_pool::~thread_pool() {
    unique_lock<mutex> lock(work_mutex);
    stopping = true;
    workers_activated.notify_all();
    lock.unlock();

    while (!workers.empty()) {
        workers.back().join();
        workers.pop_back();
    }
}

void thread_pool::execute(void(scene::*func)(size_t thread_index)) {
    unique_lock<mutex> lock(work_mutex);
    work_function = func;
    num_workers_running = workers.size();

    for (unsigned i = 0; i < worker_running.size(); ++i)
        worker_running[i] = true;

    workers_activated.notify_all();
    workers_completed.wait(lock);
    work_function = nullptr;
}

void thread_pool::loop(size_t thread_index) {
    unique_lock<mutex> lock(work_mutex);

    while (!stopping) {
        if (!worker_running[thread_index])
            workers_activated.wait(lock);

        if (worker_running[thread_index] && work_function) {
            lock.unlock();
            (parent_scene->*work_function)(thread_index);
            lock.lock();
            worker_running[thread_index] = false;

            if (!(--num_workers_running))
                workers_completed.notify_one();
        }
    }
}

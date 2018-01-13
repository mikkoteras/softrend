#ifndef THREAD_POOL
#define THREAD_POOL

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class scene;

class thread_pool {
public:
    thread_pool() = delete;
    thread_pool(scene *parent_scene, size_t num_workers);
    ~thread_pool();

    void execute(void(scene::*func)(size_t thread_index));

private:
    void loop(size_t thread_index);

    scene *parent_scene;
    std::vector<std::thread> threads;

    std::mutex work_mutex;
    std::condition_variable threads_activated;
    std::condition_variable threads_completed;
    std::vector<bool> thread_busy;
    size_t num_threads_busy = 0;
    void(scene::*work_function)(size_t thread_index) = nullptr;
    bool stopping = false;
};

#endif
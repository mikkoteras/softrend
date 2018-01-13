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
    thread_pool(scene *parent_scene, int num_workers);
    ~thread_pool();

    void execute(void(scene::*func)(size_t thread_index));

private:
    void loop(size_t thread_index);

    scene *parent_scene;
    std::vector<std::thread> workers;

    std::mutex work_mutex;
    std::condition_variable workers_activated;
    std::condition_variable workers_completed;
    std::vector<bool> worker_running;
    size_t num_workers_running = 0;
    void(scene::*work_function)(size_t thread_index) = nullptr;
    bool stopping = false;
};

#endif

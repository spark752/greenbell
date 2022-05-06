#ifndef GB_THREAD_POOL_H
#define GB_THREAD_POOL_H

#include <cstddef>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>

namespace Greenbell {

class ThreadPool {
    // Jobs are "void func(void)" but may consume parameters via lambda capture
    // or std::bind
    using job_t = std::function<void(void)>;

  public:
    ThreadPool(std::size_t thread_count = 1);
    ~ThreadPool();

    // No copies or moves
    ThreadPool(const ThreadPool& rhs) = delete;
    ThreadPool& operator=(const ThreadPool& rhs) = delete;
    ThreadPool(ThreadPool&& rhs) = delete;
    ThreadPool& operator=(ThreadPool&& rhs) = delete;
    
    // Dispatch has "copy job" and "move job" versions
    void dispatch(const job_t& op);
    void dispatch(job_t&& op);

  private:
    std::mutex mutex_;
    std::queue<job_t> q_;
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
    bool quit_ = false;

    void thread_handler();
};

} // namespace Greenbell
#endif

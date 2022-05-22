#include "thread_pool.h"
#include "log.h"
#include "cmake_config.h" /* DEBUG_WRAPPERS */

// DEBUG_WRAPPERS messages use std::count instead of fmt::print since fmt
// might throw exceptions

namespace Greenbell {

ThreadPool::ThreadPool(std::size_t thread_count) : threads_{thread_count} {
    #ifdef DEBUG_WRAPPERS
    std::cout << "ThreadPool ctor\n";
    #endif

    Log::Write(LOG_TRACE, "Creating thread pool of %d threads\n", thread_count);
    for (auto& t : threads_) {
        t = std::thread(&ThreadPool::thread_handler, this);
    }
}

ThreadPool::~ThreadPool() {
    #ifdef DEBUG_WRAPPERS
    std::cout << "ThreadPool dtor\n";
    #endif

    // Lock mutex, set quit flags, notify threads
    {
        std::lock_guard<std::mutex> lock{mutex_};
        quit_ = true;
        cv_.notify_all();
    }

    // Wait for each thread to finish
    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void ThreadPool::dispatch(const job_t& op) {
    #ifdef DEBUG_WRAPPERS
    std::cout << "ThreadPool dispatch copy version\n";
    #endif

    std::lock_guard<std::mutex> lock{mutex_};
    q_.push(op);
    cv_.notify_one();
}

void ThreadPool::dispatch(job_t&& op) {
    #ifdef DEBUG_WRAPPERS
    std::cout << "ThreadPool dispatch move version\n";
    #endif

    std::lock_guard<std::mutex> lock{mutex_};
    q_.push(std::move(op));
    cv_.notify_one();
}

void ThreadPool::thread_handler() {
    std::unique_lock<std::mutex> lock{mutex_};
    while(!quit_) {
        // Wait checks the predicate first so won't wait if there are jobs in
        // the queue or the quit flag is set. Mutex must be locked before
        // calling wait. Wait will unlock while waiting and relock on wakeup.
        cv_.wait(lock, [this](){
            return (!q_.empty() || quit_); // If false will continue waiting
        });

        // Mutex is now locked
        if (!q_.empty() && !quit_) {
            // Grab a job from the queue and run it
            auto job_function = std::move(q_.front());
            q_.pop();
            lock.unlock(); // Mutex should be unlocked while running job...
            job_function();
            lock.lock(); // ...but must be relocked for next wait
        }
    }
}

} // namespace Greenbell

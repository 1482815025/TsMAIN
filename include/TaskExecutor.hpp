#ifndef TASKEXECUTOR_HPP
#define TASKEXECUTOR_HPP

#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <functional>
#include <condition_variable>

#ifdef TS_EXPORTS
#define TS_API __declspec(dllexport)
#elif defined(TS_STATIC)
#define TS_API
#else
#define TS_API __declspec(dllimport)
#endif

extern TS_API std::atomic<bool> stopFlag;
extern TS_API std::atomic<bool> HeaderInit;
extern TS_API std::shared_mutex mtxLog;
extern TS_API std::ofstream logFile;

class TaskExecutor {
public:
    TaskExecutor() : stop(false), taskCount(0) {
        workerThread = std::thread(&TaskExecutor::processTasks, this);
    }

    ~TaskExecutor() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_one();
        workerThread.join();
    }

    void enqueueTask(const std::function<void()>& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(task);
            ++taskCount;
        }
        condition.notify_one();
    }

    void waitForAllTasks() {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this] { return taskCount == 0; });
    }

private:
    void processTasks() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = tasks.front();
                tasks.pop();
            }
            task();
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                --taskCount;
                if (taskCount == 0) condition.notify_all();
            }
        }
    }

    std::thread workerThread;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    int taskCount;
};

#endif // TASKEXECUTOR_HPP
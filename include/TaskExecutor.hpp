/*----------------------------------------------------------------------------
| File        : TaskExecutor.hpp
| Project     : TsAPI
|
| Description : task queuing system for handling time-consuming tasks.
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng
| Date        : 2024/12/22
|---------------------------------------------------------------------------*/

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

#ifdef _WIN32
#  ifdef TS_EXPORTS
#    define TS_API __declspec(dllexport)
#  elif defined(TS_STATIC)
#    define TS_API
#  else
#    define TS_API __declspec(dllimport)
#  endif
#else
#  define TS_API
#endif


// External variable declarations
TS_API extern std::atomic<bool> stopFlag;
TS_API extern std::atomic<bool> HeaderInit;
TS_API extern std::atomic<bool> clockResetFlag;
TS_API extern std::shared_mutex mtxLog;
TS_API extern std::ofstream logFile;

// TaskExecutor class declaration
class TS_API TaskExecutor {
public:
    TaskExecutor();
    ~TaskExecutor();

    void enqueueTask(const std::function<void()>& task);
    void waitForAllTasks();

private:
    void processTasks();

    std::thread workerThread;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    int taskCount;
};

#endif // TASKEXECUTOR_HPP

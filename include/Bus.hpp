/*----------------------------------------------------------------------------
| File        : Bus.hpp
| Project     : TsAPI
|
| Description : General father class for all bus
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef BUS_HPP
#define BUS_HPP

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdio.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <list>
#include "observer.hpp"
#include "TaskExecutor.hpp"
#include "vxlapi.h"

#ifdef TS_EXPORTS
#define TS_API __declspec(dllexport)
#elif defined(TS_STATIC)
#define TS_API
#else
#define TS_API __declspec(dllimport)
#endif

extern TS_API TaskExecutor LogTask;

inline TS_API std::string timeNow() {
    char buffer[100];
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S. %p %Y", &now_tm);
    std::string formattedTime = std::string(buffer);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch()).count() % 1000;
    size_t dotPos = formattedTime.find(".");
    formattedTime.insert(dotPos + 1, std::to_string(milliseconds));
    return formattedTime;
}

class TS_API IBus {
private:
    std::list<ObserverM*> monitors;

    void LogHeader() {
        std::string logTime = timeNow();
        logFile << "date " << logTime << std::endl;
        logFile << "base hex timestamps absolute" << std::endl;
        logFile << "// version 17.5.0" << std::endl;
        logFile << "// Measurement UUID:" << std::endl;
        logFile << "Begin triggerBlock " << logTime << std::endl;
    }

protected:
    XLuint64 measurementStartTime;
    bool logFlag;

public:
    IBus(bool flag = false) : logFlag(flag) {
        if (flag) {
            if (!HeaderInit) {
                std::unique_lock<std::shared_mutex> lock(mtxLog);
                logFile.open("logFile.asc", std::ios::out);
                LogHeader();
                HeaderInit.store(true);
            }
        }
    }

    void attachMonitor(ObserverM* observer) {
        monitors.push_back(observer);
    }

    void detachMonitor(ObserverM* observer) {
        monitors.remove(observer);
    }

    void notifyMonitor(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) {
        if (monitors.empty()) return;
        for (auto* observer : monitors) {
            observer->updatePayload(id_length_dir, payload);
        }
    }

    virtual XLstatus GoOffBus() = 0;
    virtual ~IBus() {
        std::unique_lock<std::shared_mutex> lock(mtxLog);
        if (logFile.is_open()) {
            logFile << "End TriggerBlock";
            logFile.close();
        }
    };    
};

#endif // BUS_HPP
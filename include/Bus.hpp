/*----------------------------------------------------------------------------
| File        : Bus.hpp
| Project     : TsAPI
|
| Description : General father class for all bus
|-----------------------------------------------------------------------------
| Version     : 3.0
| Author      : Hao Zheng
| Date        : 2025/2/20
|---------------------------------------------------------------------------*/

#ifndef BUS_HPP
#define BUS_HPP

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
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

// External variable declarations
TS_API extern std::shared_ptr<TaskExecutor> LogTask;
TS_API extern std::array<int, 16> dlcToLengthMap;

// Global Hardware Configuration
TS_API extern XLdriverConfig xlDrvConfig;

// Global Vector Driver Functions
TS_API XLstatus HardwareInit();
TS_API std::vector<int> getCANCapChannels();
TS_API std::vector<int> getCANFDCapChannels();
TS_API std::vector<int> getLINCapChannels();

TS_API std::string timeNow();
TS_API int dlcToLength(int dlc);

class TS_API IBus {
private:
    std::list<ObserverM*> monitors;

protected:
    XLuint64 measurementStartTime;
    bool logFlag;
    void LogHeader();
    void logInit();

public:
    // Constructor
    IBus(bool flag = false);

    // Attach and detach observer monitors
    void attachMonitor(ObserverM* observer);
    void detachMonitor(ObserverM* observer);

    // Notify monitors with payload and id_length_dir information
    void notifyMonitor(XLcanRxEvent xlCanfdRxEvt);
    void notifyMonitor(XLevent xlEvent);
    void notifyDIOMonitor(unsigned int data);
    void notifyAINMonitor(XL_IO_ANALOG_DATA data);

    // Pure virtual method for derived classes to implement
    virtual XLstatus GoOffBus() = 0;

    // Destructor
    virtual ~IBus() = 0;
};

#endif // BUS_HPP

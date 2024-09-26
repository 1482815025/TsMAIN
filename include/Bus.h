/*----------------------------------------------------------------------------
| File        : Bus.h
| Project     : TsAPI
|
| Description : General father class for all bus
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef BUS_H
#define BUS_H

#include <windows.h>
#include <iostream>
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
#include "vxlapi.h"

#ifdef TS_EXPORTS
#define TS_API __declspec(dllexport)
#elif defined(TS_STATIC)
#define TS_API
#else
#define TS_API __declspec(dllimport)
#endif

extern TS_API std::atomic<bool> stopFlag;

class TS_API IBus {
    std::list<Observer*> observers;

public:
    void attach(Observer* observer) {
        observers.push_back(observer);
    }

    void detach(Observer* observer) {
        observers.remove(observer);
    }

    void notify(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) {
        if (observers.empty()) return;
        for (auto* observer : observers) {
            observer->update(id_length_dir, payload);
        }
    }
    virtual XLstatus GoOffBus() = 0;
    virtual ~IBus() {};    

};

#endif // BUS_H
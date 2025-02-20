/*----------------------------------------------------------------------------
| File        : observer.hpp
| Project     : TsAPI
|
| Description : Observer mode support for all bus
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng
| Date        : 2024/9/10
|---------------------------------------------------------------------------*/


#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <vector>
#include "vxlapi.h"

class ObserverM {
public:
    virtual void updatePayload(XLcanRxEvent xlCanfdRxEvt) = 0;
    virtual void updatePayload(XLevent xlEvent) = 0;
    virtual void updateDIOPayload(unsigned int data) = 0;
    virtual void updateAINPayload(XL_IO_ANALOG_DATA data) = 0;
    virtual ~ObserverM() {}
};

#endif /* OBSERVER_HPP */
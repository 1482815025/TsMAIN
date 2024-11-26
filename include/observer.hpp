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

class ObserverM {
public:
    virtual void updatePayload(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) = 0;
    virtual ~ObserverM() {}
};

#endif /* OBSERVER_HPP */
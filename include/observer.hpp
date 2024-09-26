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

class Observer {
public:
    virtual void update(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) = 0;
    virtual ~Observer() {}
};

#endif /* OBSERVER_HPP */
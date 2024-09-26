/*
 *  scheduleTable.hpp
 *
 *  Created on: 07/28/2024
 *      Author: Hao Zheng
 */

#ifndef scheduleTable_hpp
#define scheduleTable_hpp

#include <vector>
#include <string>
#include <optional>
#include "_dllspec.hpp"

class PARSER_API Schedule {

public:

    std::string frameName;
    unsigned int frameId;
    double delay;
    std::string unit;

};

class ScheduleTable {

public:

    std::string name;
    unsigned int tableId;
    std::vector<Schedule> getSchedule() const { return scheduleTable; }
    void addSchedule(Schedule& schedule) { this->scheduleTable.push_back(schedule); }

private:

    std::vector<Schedule> scheduleTable;

};



#endif /* scheduleTable_hpp */
#ifndef GETWAY_HPP
#define GETWAY_HPP

#include "TsCANFD.hpp"
#include "sendFunctions.hpp"

class Getway : public ObserverM {
public:
    Getway();
    ~Getway();
    void setOutput(CANFD* canfd) {this->outputCANFD = canfd;}
    void setUnpassedMsgId(std::vector<unsigned long> unpassedMsgId) {this->unpassedMsgId = unpassedMsgId;}
    void setOverWriteMsgId(std::vector<unsigned long> overWriteMsgId) {this->overWriteMsgId = overWriteMsgId;}

    
    void updatePayload(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) override;

private:
    CANFD* outputCANFD;
    std::vector<unsigned long> unpassedMsgId;
    std::vector<unsigned long> overWriteMsgId;
};

#endif /* GETWAY_HPP */

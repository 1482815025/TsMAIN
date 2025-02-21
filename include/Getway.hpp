#ifndef GETWAY_HPP
#define GETWAY_HPP

#include "SendFunctions.hpp"


void IGThread(std::shared_ptr<CANFD> canfd, XLcanTxEvent* canfdMsg, unsigned int cnt, unsigned int cycletime);

class Getway : public Monitor{
public:
    Getway() : Monitor() {}
    ~Getway() {}
    void updateDIOPayload(unsigned int data) override;
    void updateAINPayload(XL_IO_ANALOG_DATA data) override;
    void setPayloads(Payloads_CAN* Payloads) {this->Payloads = Payloads;}
    void setDbcParser(DbcParser* dbc) {this->dbc = dbc;}

private:
    Payloads_CAN* Payloads;
    DbcParser* dbc;

};

#endif // GETWAY_HPP
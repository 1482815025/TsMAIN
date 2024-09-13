/*----------------------------------------------------------------------------
| File        : TsCAN.h
| Project     : TsAPI
|
| Description : CAN interface and automation related testing platform for vector hardware
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Zheng, Hao
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef TSCAN_H
#define TSCAN_H

#include <sstream>
#include "Bus.h"

#define RECEIVE_EVENT_SIZE         1        // DO NOT EDIT! Currently 1 is supported only
#define RX_QUEUE_SIZE              4096     // internal driver queue size in CAN events
#define RX_QUEUE_SIZE_FD           16384    // driver queue size for CAN-FD Rx events
#define ENABLE_CAN_FD_MODE_NO_ISO  0        // switch to activate no iso mode on a CAN FD channel

#define CH1 0
#define CH2 1

typedef struct {
    XLportHandle xlPortHandle;
    HANDLE       hMsgEvent;
} TStruct;

/**
 * \class CAN
 * 
 * \brief The CAN class represents a Controller Area Network (CAN) bus.
 * 
 * The CAN class provides functionality to initialize the CAN bus, set the bitrate, activate and deactivate the channel,
 * send messages, reset and set filters, and retrieve channel information.
 */
class TS_API CAN : public IBus
{
public:
    CAN(unsigned long baudrate = 500000) {
        m_xlbaudrate = baudrate;
        m_xlChannelMask = 0;
        m_xlChannelIndex = -1;
        rxPayloads = {};
        m_xlDrvConfig = {};
        CANInit();
        CANGoOnBus();
    };
    
    ~CAN();
    XLstatus CANInit();
    XLstatus CANGoOnBus();
    XLstatus GoOffBus() override;
    XLstatus CANSend(XLevent* xlEvent, unsigned int messageCount);
    XLstatus CANResetFilter();
    XLstatus CANSetFilter(unsigned long first_id, unsigned long last_id);
    const char* GetChannelName(int channel);
    void updateRxPayloads(s_xl_can_msg msg, std::string dir = "RX");
    void printRxPayloads();
    std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> getRxPayloads() {return rxPayloads;}
    // void getRxPayloads(std::ostream& os);

private:

    std::shared_mutex       mtxCAN;
    XLstatus canGetChannelMask();
    XLstatus canInit();
    XLstatus canCreateRxThread();
    
    XLaccess         m_xlChannelMask;        // we support only two channels
    int              m_xlChannelIndex;       // we support only two channels
    XLportHandle     m_xlPortHandle;            // and one port
    XLaccess         m_xlChannelMask_both;      // combined channel mask
    XLdriverConfig   m_xlDrvConfig;             // driver configuration
    unsigned long    m_xlbaudrate;
    HANDLE           m_hThread;
    XLhandle         m_hMsgEvent;
    int              m_bInitDone;
    char            m_AppName[XL_MAX_APPNAME + 1] = "TsCAN";               //!< Application name which is displayed in VHWconf
    // id, length, Tx/Rx, payloads
    std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> rxPayloads;

};

// DWORD     WINAPI RxThread(PVOID par, std::string& eventString);
TS_API void RxThread (CAN &can, TStruct &par);

#endif // TSCAN_H
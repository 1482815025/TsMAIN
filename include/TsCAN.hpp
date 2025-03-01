/*----------------------------------------------------------------------------
| File        : TsCAN.hpp
| Project     : TsAPI
|
| Description : CAN interface and automation related testing platform for vector hardware
|               Multi-Bus supported
|-----------------------------------------------------------------------------
| Version     : 2.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2024/12/11
|---------------------------------------------------------------------------*/

#ifndef TSCAN_HPP
#define TSCAN_HPP

#include "Bus.hpp"

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
    CAN(int channel, int appCh = 0, unsigned long baudrate = 500000, bool flag = false);
    ~CAN() override;
    std::thread receiveThread;
    XLstatus CANGoOnBus();
    XLstatus GoOffBus() override;
    XLstatus CANSend(XLevent* xlEvent, unsigned int messageCount);
    XLstatus CANResetFilter();
    XLstatus CANSetFilter(unsigned long first_id, unsigned long last_id);
    const char* GetChannelName(int channel);
    void updateRxPayloads(XLevent& xlEvent);    
    // void printRxPayloads();
    // std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> getRxPayloads() {return rxPayloads;}

private:

    // std::shared_mutex       mtxCAN;
    XLstatus canGetChannelMask();
    XLstatus HardwareInit();
    XLstatus canCreateRxThread();
    void canLogger(XLevent xlEvent);
    void RxThread();
    XLaccess         m_xlChannelMask;
    int              m_xlChannelIndex;
    XLportHandle     m_xlPortHandle;
    XLdriverConfig   m_xlDrvConfig;
    unsigned long    m_xlbaudrate;
    XLhandle         m_hMsgEvent;
    int              m_bInitDone;
    unsigned int  appChannel;
    char            m_AppName[XL_MAX_APPNAME + 1] = "TsAPI";
    bool            g_rxThreadRun;
    // id, length, Tx/Rx, payloads
    // std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> rxPayloads;

};

// TS_API void RxThread (CAN &can, TStruct &par);

#endif // TSCAN_HPP
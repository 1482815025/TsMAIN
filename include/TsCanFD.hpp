/*----------------------------------------------------------------------------
| File        : TsCANFD.hpp
| Project     : TsAPI
|
| Description : CANFD interface and automation related testing platform for vector hardware
|               Multi-Bus supported
|-----------------------------------------------------------------------------
| Version     : 2.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2024/12/22
|---------------------------------------------------------------------------*/

#ifndef TSCANFD_HPP
#define TSCANFD_HPP

#include <array>
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
} TStruct_FD;

////////////////////////////////////////////////////////////////////////////

//This class is used for CANFD

//////////////////////////////////////////////////////////////////////////// 
class TS_API CANFD : public IBus
{
public:
    CANFD(int channel, int appCh = 0, unsigned long arbitrationBitRate = 500000, unsigned long dataBitRate = 2000000, bool flag = false);
    ~CANFD();
    std::thread receiveThreadfd;
    XLstatus CANFDGoOnBus();
    XLstatus GoOffBus() override;
    XLstatus CANFDSend(XLcanTxEvent* canTxEvt, unsigned int messageCount);
    XLstatus CANFDResetFilter();
    XLstatus CANFDSetFilter(unsigned long first_id, unsigned long last_id);
    void updateRxPayloads(XLcanRxEvent& xlCanfdRxEvt, std::string dir = "Rx");
    std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> getRxPayloads() {return rxPayloads;}
    void printRxPayloads();
    

private:
    
    std::shared_mutex mtxCANFD;
    XLstatus canfdGetChannelMask();
    XLstatus HardwareInit();
    XLstatus canfdCreateRxThread();
    void canfdLogger(const XLcanRxEvent& xlCanfdRxEvt, const std::string dir);
    void RxThread_CANFD();

    XLaccess         m_xlChannelMask;        //!< we support only two channels
    int              m_xlChannelIndex;       //!< we support only two channels
    XLportHandle     m_xlPortHandle;            //!< and one port
    XLdriverConfig   m_xlDrvConfig;
    XLcanFdConf   fdParams;
    HANDLE           m_hThread;
    XLhandle         m_hMsgEvent;
    int              m_bInitDone;
    unsigned int    appChannel;
    char             m_AppName[XL_MAX_APPNAME + 1] = "TsAPI";
    bool g_fdrxThreadRun = true;
    // id, length, Tx/Rx, payloads
    std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> rxPayloads;

};

// DWORD     WINAPI RxThread_CANFD(PVOID par);

#endif // TSCANFD_HPP
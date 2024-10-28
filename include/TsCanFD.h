/*----------------------------------------------------------------------------
| File        : TsCANFD.h
| Project     : TsAPI
|
| Description : CANFD interface and automation related testing platform for vector hardware
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef TSCANFD_H
#define TSCANFD_H

#include <array>
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
} TStruct_FD;

////////////////////////////////////////////////////////////////////////////

//This class is used for CANFD

//////////////////////////////////////////////////////////////////////////// 
class TS_API CANFD : public IBus
{
public:
    /**
     * \brief Constructor of CANFD.
     * 
     * \param arbitrationBitRate Arbitration CAN bus timing for nominal / arbitration bit rate in bit/s.
     * \param dataBitRate CAN bus timing for data bit rate in bit/s. Range: dataBitRate >= max(arbitrationBitRate, 25000).
     * \param flag Flag of whether the measurement should be logged.
     */
    CANFD(unsigned long arbitrationBitRate = 500000, unsigned long dataBitRate = 2000000, bool flag = false)
    : IBus(flag),
    m_xlChannelMask(0),
    m_xlChannelIndex(0),
    m_xlPortHandle(-1),
    rxPayloads(),
    m_xlDrvConfig()
    {
        memset(&fdParams, 0, sizeof(fdParams));
        // arbitration bitrate
        fdParams.arbitrationBitRate = arbitrationBitRate;
        fdParams.tseg1Abr = 6;
        fdParams.tseg2Abr = 3;
        fdParams.sjwAbr = 2;

        // data bitrate
        fdParams.dataBitRate = dataBitRate;
        fdParams.tseg1Dbr = 6;
        fdParams.tseg2Dbr = 3;
        fdParams.sjwDbr = 2;

        CANFDInit();
        CANFDGoOnBus();
    };
    
    ~CANFD();
    std::thread receiveThreadfd;
    XLstatus GoOffBus() override;
    XLstatus CANFDSend(XLcanTxEvent* canTxEvt, unsigned int messageCount);
    XLstatus CANFDResetFilter();
    XLstatus CANFDSetFilter(unsigned long first_id, unsigned long last_id);
    void updateRxPayloads(XLcanRxEvent& xlCanfdRxEvt, std::string dir = "Rx");
    std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> getRxPayloads() {return rxPayloads;}
    void printRxPayloads();

private:
    std::shared_mutex mtxCANFD;
    void canfdLogger(const XLcanRxEvent& xlCanfdRxEvt, const std::string dir);
    XLstatus CANFDInit();
    XLstatus CANFDGoOnBus();
    XLstatus canfdGetChannelMask();
    XLstatus HardwareInit();
    XLstatus canfdCreateRxThread();

    XLaccess         m_xlChannelMask;        //!< we support only two channels
    int              m_xlChannelIndex;       //!< we support only two channels
    XLportHandle     m_xlPortHandle;            //!< and one port
    XLaccess         m_xlChannelMask_both;
    XLdriverConfig   m_xlDrvConfig;

    XLcanFdConf   fdParams;



    HANDLE           m_hThread;
    XLhandle         m_hMsgEvent;
    int              m_bInitDone;

    char             m_AppName[XL_MAX_APPNAME + 1] = "TsCANFD";               //!< Application name which is displayed in VHWconf
    // id, length, Tx/Rx, payloads
    std::vector<std::pair<std::pair<std::vector<unsigned long>, std::string>, std::vector<unsigned char>>> rxPayloads;

};

// DWORD     WINAPI RxThread_CANFD(PVOID par);
TS_API void RxThread_CANFD (CANFD &canfd, TStruct_FD &par);

#endif // TSCANFD_H
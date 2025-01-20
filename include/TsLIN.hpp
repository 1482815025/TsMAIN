/*----------------------------------------------------------------------------
| File        : TsLIN.h
| Project     : TsAPI
|
| Description : LIN interface and automation related testing platform for vector hardware
|               Multi-Bus supported
|-----------------------------------------------------------------------------
| Version     : 2.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2024/12/11
|---------------------------------------------------------------------------*/

#ifndef TSLIN_HPP
#define TSLIN_HPP

#include "Bus.hpp"
#include "vxlapi.h"

#define RECEIVE_EVENT_SIZE       1     // DO NOT EDIT! Currently 1 is supported only
#define MASTER                   1     //!< channel is a master
#define SLAVE                    0     //!< channel is a slave
#define DEFAULT_LIN_DLC          8     //!< default DLC for master/slave
#define DEFAULT_LIN_BAUDRATE 19200     //!< default LIN baudrate

//global vriables

typedef struct {
    XLportHandle xlPortHandle;
    HANDLE      hMsgEvent;
} TStruct_LIN;

TS_API typedef struct{
    unsigned int    ID;
    unsigned int    dlc;
    std::string     dir;
    std::vector<unsigned char>  payload;
} rxPayload;

class TS_API LIN : public IBus
{
public:
    LIN(int channel, int appCh = 0, int baudrate = DEFAULT_LIN_BAUDRATE, unsigned int LinVersion = XL_LIN_VERSION_2_0, bool isMaster = true, bool flag = false);
    ~LIN();
    std::thread      receiveThreadLin;
    XLstatus            HardwareInit();
    XLstatus            LINInit();
    XLstatus                LINGoOnBus();
    XLstatus                GoOffBus() override;
    XLstatus                linSetSlave(unsigned int linID, unsigned char data[8], unsigned int dlc);
    XLstatus                linCreateRxThread();
    XLstatus                LINSendMasterReq(unsigned int linID);
    XLstatus                updateRxPayloads(XLevent& pEvent);
    void                    printRxPayloads();
    std::vector<rxPayload>  getRxPayloads() { return linRxPayloads; }

private:

    std::shared_mutex   mtxLIN;
    
    XLstatus            linGetChannelMask();
    
    XLstatus            linInitMaster();
    XLstatus            linInitSlave();
    void                RxThread_LIN();
    void                linLogger(const XLevent& xlEvent, const std::string dir);

    XLaccess            m_xlChannelMask;
    int                 m_xlChannelIndex;
    XLportHandle        m_xlPortHandle;
    XLhandle            m_hMsgEvent;
    int                 m_baudrate;
    unsigned int        m_LinVersion;
    char                m_AppName[XL_MAX_APPNAME + 1] = "TsAPI";
    unsigned int        appChannel;
    XLdriverConfig      m_xlDrvConfig;
    bool                m_bInitDone;
    BOOL                g_bThreadRun_LIN;
    bool masterFlag;
    // id, dlc, Tx/Rx, payload
    std::vector<rxPayload> linRxPayloads;
};

// TS_API void RxThread_LIN(LIN& lin, TStruct_LIN& pTh);

#endif // TSLIN_HPP
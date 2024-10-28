/*----------------------------------------------------------------------------
| File        : TsLIN.h
| Project     : TsAPI
|
| Description : LIN interface and automation related testing platform for vector hardware
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef TSLIN_H
#define TSLIN_H

#include "Bus.h"
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
    /**
     * \brief Constructor of LIN.
     * 
     * \param baudrate This value specifies the real bit rate.
     * \param LinVersion LIN protocol version.
     * \param isMaster Indicates whether the channel is a master or a slave.
     * \param flag Flag of whether the measurement should be logged.
     */
    LIN(int baudrate = DEFAULT_LIN_BAUDRATE, unsigned int LinVersion = XL_LIN_VERSION_2_0, bool isMaster = true, bool flag = false)
    : IBus(flag),
    m_baudrate(baudrate),
    m_LinVersion(LinVersion),
    m_xlPortHandle(-1),
    m_xlChannelMask(0),
    m_xlChannelIndex(0)
    {
        LINGetDevice();
        LINInit(isMaster);
    }
    virtual ~LIN();
    std::thread      receiveThreadLin;
    XLstatus                GoOffBus() override;
    XLstatus                linSetSlave(unsigned int linID, unsigned char data[8], unsigned int dlc);
    XLstatus                linCreateRxThread();
    XLstatus                LINSendMasterReq(unsigned int linID);
    XLstatus                updateRxPayloads(XLevent* pEvent);
    void                    printRxPayloads();
    std::vector<rxPayload>  getRxPayloads() { return linRxPayloads; }

private:

    std::shared_mutex   mtxLIN;
    void linLogger(const XLevent* xlEvent, const std::string dir);
    XLstatus            LINGetDevice();
    XLstatus            LINInit(bool isMaster = true);
    XLstatus            linGetChannelMask();
    XLstatus            linInitMaster();
    XLstatus            linInitSlave();
    XLstatus            linActivateChannel();
    XLaccess            m_xlChannelMask;
    int                 m_xlChannelIndex;
    XLportHandle        m_xlPortHandle;
    XLhandle            m_hMsgEvent;
    HANDLE              m_hThread;
    int                 m_baudrate;
    unsigned int        m_LinVersion;
    char                m_AppName[XL_MAX_APPNAME + 1] = "TsLIN";
    bool                m_bInitDone;
    // id, dlc, Tx/Rx, payload
    std::vector<rxPayload> linRxPayloads;
};

TS_API void RxThread_LIN(LIN& lin, TStruct_LIN& pTh);

#endif // TSLIN_H